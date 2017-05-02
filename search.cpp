#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <map>
#include <algorithm>

using namespace std;

struct entry
{
    char * word;
    int dPointer;
    int dLength;
    int nt;

    entry(string w, int x, int y, int z)
    {
        word = new char [w.length()+1];
        strcpy (word, w.c_str());
        dPointer = x;
        dLength = y;
        nt = z;
    }
};

vector<entry> dictionary;
vector<int> docLengths;

/* Load the files produced by the indexer into memory */
void loadData(string term)
{
    ifstream in("dictionary.txt",ifstream::in);
    string line;

    //Load dictionary from file.

    /*
    if(isalpha(term.at(0)))
    {
        ifstream inP ("dictPointers.txt",ifstream::in);
        string jump;
        for(int i = 0; i < (term.at(0) - 'A' + 1); i++){
            getline(inP, jump);
        }
        inP.close();
        //in.seekg(8001460);
        cout << atoi(jump.c_str()) << "\n";
        in.seekg( atoi(jump.c_str()));

        while( line.length() > 0 && line.at(0) != term.at(0))
        {
            getline(in, line);
        }
        getline(in, line);
        getline(in, line);
    }*/
    while (getline(in, line) && line != term)
    {
        getline(in, line);
        getline(in, line);
        getline(in, line);
    }

    string a = line;

    getline(in, line);
    int b = atoi(line.c_str());

    getline(in, line);
    int c = atoi(line.c_str());

    getline(in, line);
    int d = atoi(line.c_str());

    //cout << a << "," << b << "," << c << "," << d << "\n";
    dictionary.push_back( entry(a,b,c,d) );

    in.close();
}

/* Return the position of word in the dictionary, return -1 if the word is not found */
int search(string word)
{
    int begin = 0;
    int end = dictionary.size();
    int mid = end/2;

    while ((end - begin) > 10)
    {
        if(dictionary.at(mid).word > word ){
            end = mid;
            mid = end/2;
        } else if (dictionary.at(mid).word < word ) {
            begin = mid;
            mid = begin + ((end-begin)/2);
        } else {
            return mid;
        }
    }

    for(int i = begin; i < end; i++){
        if(dictionary.at(i).word == word){
            return i;
        }
    }

    return -1;
}

/* Retrieve the postings list for the word at position i in the dictionary */
string getPosting(int i)
{
    int loc = dictionary.at(i).dPointer;
    int len = dictionary.at(i).dLength;

    ifstream infile ("index.txt",std::ifstream::binary);
    infile.seekg(loc);

    char* buffer = new char[len];
    infile.read (buffer,len);
    buffer[len] = '\0';

    string out = buffer;

    delete[] buffer;
    infile.close();

    return out;
}

bool myComparison(const pair<int,double> &a,const pair<int,double> &b)
{
       return a.second>b.second;
}

int main()
{

    string line;
    ifstream inL("doclengths.txt");
    while (getline(inL, line))
    {
        docLengths.push_back( atoi(line.c_str()) );
    }
    inL.close();

    string terms;
    char tc[100];
    cin.getline( tc, 100);
    terms = tc;
    delete[] tc;
    istringstream sst (terms);
    string term;
    map <int, double> result;

    while( getline(sst, term, ' ') ){
        loadData(term);
        int i = search(term);
        if(i >= 0) //Word exists in dictionary.
        {
            string posting = getPosting(i);
            istringstream ss (posting);
            string token;

            double nt = dictionary[i].nt;
            double n = docLengths.size();
            double idf = 1/(nt/n);


            while(getline(ss, token, ',') && token.length() > 0)
            {

                int dn = (atoi(token.c_str())-1);
                getline(ss, token, ',');
                int fr = atoi(token.c_str());

                double freq = (double)fr / (double)docLengths[dn];
                double rank = freq*idf;

                map<int, double>::iterator it = result.find(dn);
                if(it != result.end()){
                    double oFreq = it->second;
                    result.erase (it);
                    result.emplace( dn, rank+oFreq );
                } else {
                    result.emplace( dn, rank );
                }

            }

        }
    }

    vector<pair<int,double> > v(result.size());
    copy(result.begin(), result.end(), v.begin());

    sort(v.begin(),v.end(),myComparison);

    ifstream infile ("doclist.txt",std::ifstream::binary);
    for(pair<int, double> x: v){
        infile.seekg((get<0>(x) * 15));
        char* buffer = new char[15];
        infile.read (buffer,15);
        buffer[14] = '\0';
        cout << buffer;
        cout << "\t" << get<1>(x) << "\n";
        delete[] buffer;

    }
    infile.close();

    return 0;
}
