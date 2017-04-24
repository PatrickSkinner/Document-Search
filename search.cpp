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
    int dft;
    int dPointer;
    int dLength;

    entry(string w, int x, int y, int z)
    {
        word = new char [w.length()+1];
        strcpy (word, w.c_str());
        dPointer = x;
        dLength = y;
        dft = z;
    }

    bool operator==(const entry& rhs) const { word == rhs.word; }
};

int search(string word, vector<entry> dictionary)
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

string getPosting(int i, vector<entry> dictionary)
{
    int loc = dictionary.at(i).dPointer;
    int len = dictionary.at(i).dLength;

    ifstream infile ("index.txt",std::ifstream::binary);
    infile.seekg(loc);

    char* buffer = new char[len-1];
    infile.read (buffer,len-1);

    string out = buffer;

    delete[] buffer;
    infile.close();

    return out;
}

int main()
{
    ifstream in("dictionary.txt");
    string line;
    vector< entry > dictionary;

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

    int i = search("rosenfield", dictionary);

    if(i >= 0)
    {
        multimap <int, string> result;
        string posting = getPosting(i, dictionary);
        istringstream ss (posting);
        string token;
        ifstream infile ("doclist.txt",std::ifstream::binary);

        while(getline(ss, token, ',') && token.length() > 0)
        {
            string dn = token;
            getline(ss, token, ',');

            string fr = token;

            infile.seekg((atoi(dn.c_str())-1)* 15);
            char* buffer = new char[14];
            infile.read (buffer,13);
            buffer[13] = '\0';
            string out = buffer;
            delete[] buffer;

            result.emplace( atoi(fr.c_str()) , out );
        }

        infile.close();

        for (auto iter = result.rbegin(); iter != result.rend(); ++iter) {
            cout << iter->second << "\t" << iter->first << "\n";
        }

    } else {
        cout << "Term Not Found.\n";
    }

    return 0;
}
