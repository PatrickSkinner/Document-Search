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
    ifstream in("dictionary.txt");
    string line;

    //Load dictionary from file.
    //in.seekg(8001460);
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

    string terms = "THESIS";
    istringstream sst (terms);
    string term;
    map <string, double> result;
    map <int, double> result2;

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

                /*
                infile.seekg((dn * 15));
                char* buffer = new char[15];
                infile.read (buffer,15);
                buffer[14] = '\0';
                string out = buffer;
                //cout << out << "\n";
                delete[] buffer;
                */

                double freq = (double)fr / (double)docLengths[dn];
                double rank = freq*idf;

                //result2.emplace( dn, rank );

                map<int, double>::iterator it = result2.find(dn);
                if(it != result2.end()){
                    double oFreq = it->second;
                    result2.erase (it);
                    result2.emplace( dn, rank+oFreq );
                } else {
                    result2.emplace( dn, rank );
                }
                //delete[] buffer;

            }

        }
    }

    vector<pair<int,double> > v(result2.size());
    copy(result2.begin(), result2.end(), v.begin());

    /*
    for (auto iter = result2.rbegin(); iter != result2.rend(); ++iter) {
            cout << iter->first << "\t" << iter->second << "\n";
    }*/

    sort(v.begin(),v.end(),myComparison);

    ifstream infile ("doclist.txt",std::ifstream::binary);
    for(pair<int, double> x: v){
        //cout << get<0>(x)
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
