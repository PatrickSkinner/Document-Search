#include <iostream>
#include <fstream>
#include <map>
#include <vector>

using namespace std;

struct posting
{
    int docNo;
    int freq;

    posting(int d, int f)
    {
        docNo = d;
        freq = f;
    }
};

map<string, vector<posting> > index;
vector<string> docList;
vector<int> docLengths;

void read()
{
    bool isDocNo = true;

    string currentDoc = "";
    string line;

    ifstream in("parsed.txt");

    if (in.is_open())
    {
        while (getline(in, line))
        {
            if(isDocNo == true && line.length() > 1) //The line is a document ID.
            {
                currentDoc = line;
                docList.push_back(currentDoc);
                docLengths.push_back(0);
                isDocNo = false;
            } else if (line.length() < 1){ //The next line is a document ID, set flag.
                isDocNo = true;
            } else { //Is a word
                docLengths.back()++;

                if(index.find(line) == index.end()) //Word is not already in the index
                {
                    posting p (docList.size(), 1);
                    vector<posting> list {p};
                    index.emplace(line, list);
                } else { //Word is already in the index
                    bool f = false;

                    //Check if the document is in the postings list already, increment frequency if it is.
                    if(index.find(line)->second.back().docNo == docList.size()){
                        index.find(line)->second.back().freq++;
                        f = true;
                    }

                    //Document was not found in postings list. Add it.
                    if(f == false)
                    {
                        posting p (docList.size(), 1);
                        index.find(line)->second.push_back(p);
                    }
                }
            }
        }
        in.close();
    }
}

void write()
{
    std::ofstream outD ("dictionary.txt",std::ofstream::out);
    std::ofstream outI ("index.txt",std::ofstream::out);

    if(outD.is_open() && outI.is_open())
    {
        for (auto& x: index)
        {
            outD << x.first << "\n";
            int iStart = outI.tellp();
            outD << outI.tellp() << "\n";

            int i = 0;
            for (auto& y: x.second)
            {
                outI << y.docNo << "," << y.freq << ",";
                i++;
            }

            outD << (int) outI.tellp() - iStart << "\n";
            outD << i << "\n";
        }
    }
    outD.close();
    outI.close();

    std::ofstream outDL ("doclist.txt",std::ofstream::out);
    for(auto& x: docList)
    {
        outDL << x << "\n";
    }
    outDL.close();

    std::ofstream outDLn ("doclengths.txt",std::ofstream::out);
    for (auto& x: docLengths){
        outDLn << x << "\n";
    }
    outDLn.close();
}

int main()
{
    read();
    write();

    return 0;
}
