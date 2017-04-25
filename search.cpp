#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <map>

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
void loadData()
{
    ifstream in("dictionary.txt");
    string line;

    //Load dictionary from file.
    while (getline(in, line))
    {
        string a = line;

        getline(in, line);
        int b = atoi(line.c_str());

        getline(in, line);
        int c = atoi(line.c_str());

        getline(in, line);
        int d = atoi(line.c_str());

        dictionary.push_back( entry(a,b,c,d) );
    }
    in.close();

    //Load document lengths from file.
    ifstream inL("doclengths.txt");
    while (getline(inL, line))
    {
        docLengths.push_back( atoi(line.c_str()) );
    }
    inL.close();
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
    infile.read (buffer,len-1);
    buffer[len] = '\0';

    string out = buffer;

    delete[] buffer;
    infile.close();

    return out;
}

int main()
{
    loadData();

    int i = search("ROSENFIELD");

    if(i >= 0) //Word exists in dictionary.
    {
        multimap <double, string> result;
        string posting = getPosting(i);
        istringstream ss (posting);
        string token;

        double nt = dictionary[i].nt;
        double n = docLengths.size();
        double idf = 1/(nt/n);

        ifstream infile ("doclist.txt",std::ifstream::binary);
        while(getline(ss, token, ',') && token.length() > 0)
        {
            int dn = (atoi(token.c_str())-1);
            getline(ss, token, ',');
            int fr = atoi(token.c_str());

            infile.seekg(dn * 16);
            char* buffer = new char[15];
            infile.read (buffer,15);
            buffer[14] = '\0';
            string out = buffer;
            delete[] buffer;

            double freq = (double)fr / (double)docLengths[dn];
            double rank = freq*idf;

            result.emplace( rank , out );
        }
        infile.close();

        for (auto iter = result.rbegin(); iter != result.rend(); ++iter) {
            cout << iter->second << "\t" << iter->first << "\n";
        }

        return 0;

    } else {

        cout << "Term Not Found.\n";
        return -1;

    }
}
