#include <iostream>
#include <fstream>
#include <map>
#include <vector>

using namespace std;

struct posting
{
    int docNo;
    int freq;

    bool operator<(const posting& rhs) const { docNo < rhs.docNo; }
    bool operator>(const posting& rhs) const { docNo > rhs.docNo; }
    bool operator==(const posting& rhs) const { docNo == rhs.docNo; }

    posting(int d, int f)
    {
        docNo = d;
        freq = f;
    }
};

void write(map<string, vector<posting> > index, vector<string> docList)
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

            outD << outI.tellp() - iStart << "\n";
            outD << i << "\n";
        }
    }
    outD.close();
    outI.close();
}

int main()
{
    bool isDocNo = true;

    string currentDoc = "";
    string line;

    map<string, vector<posting> > index;
    vector<string> docList;
    vector<int> docLengths;

    ifstream in("parsed.txt");

    if (true)
    {
        cout << "file open \n";
        while (getline(in, line))
        {
            if(isDocNo == true && line.length() > 1)
            {
                currentDoc = line;
                docList.push_back(currentDoc);
                docLengths.push_back(0);
                isDocNo = false;
            } else if (line.length() < 1){
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

    write(index, docList);

    /*
    for (auto& x: index)
    {
        cout << x.first << "\n";

        for (auto& y: x.second)
        {
            cout << y.docNo << "(" << y.freq << ")" << ", ";
            //cout << y.docNo << "|" << y.freq << ", ";
        }
        cout << "\n";
    }


    for (auto& x: docList)
    {
        //cout << "DocNo: " << x <<"\n";
    }
    //cout << docList.size() << "\n";
    */

    /*
    for (auto& x: index)
    {
        cout << x.first << "\n";
        cout << "1000" << "\n";
        cout << "500" << "\n";
        int i = 0;

        for (auto& y: x.second)
        {
            i++;
        }
        cout << i << "\n";
    }
    for (auto& x: docList){
        //cout << x << "\n";
    }
    //cout << "\n";
    for (auto& x: docLengths){
        //cout << x << "\n";
    }
    */

    return 0;
}
