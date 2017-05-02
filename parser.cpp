#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

string getState(string token)
{
    if( token == "<TEXT>" || token == "<P>" || token == "<LP>")
    {
         return "READ-TEXT";
    } else if (token == "<DOCNO>") {
        return "READ-DOCNO";
    } else if (token == "</DOC>") {
        cout << "\n";
        return "FIND-TAG";
    } else {
        return "ERROR";
    }
}

int main () {
    ifstream infile ("text.xml",std::ifstream::binary);
    char c;
    string state = "FIND-TAG";
    string token = "";

    while(infile.get(c)){

        if(c == '<')
        {
            state = "IN-TAG";
            token = token + c;

        } else if (state == "IN-TAG")
        {
            token = token + c;
            while(c != '>'){
                infile.get(c);
                token = token + c;
            }
            //cout << token << "\n";
            state = getState(token);
            token = "";

        } else if (state == "READ-TEXT"){

            if(c == '<')
            {
                token = "";
                state = "IN-TAG";
            }else if(isspace(c))
            {
                token = "";
            } else if (c == '&'){
                if(token != "") {cout << token << "\n"; }
                token = "";
                state = "ESCAPE-CHARACTER";
            } else {
                while(c != '<' && !isspace(c)  && c != '-' && c!='&'){
                    if(isalnum(c)) { token = token + (char)toupper(c); }
                    infile.get(c);
                }
                if(c == '&') { state = "ESCAPE-CHARACTER";}
                if(token.find_first_not_of(' ') != std::string::npos)
                {
                    cout << token << "\n";
                }
                token = "";
            }

        } else if (state == "READ-DOCNO") {
            while(c != '<')
            {
                if(!isspace(c)) { token = token + c; }
                infile.get(c);
            }
            cout << token << "\n";
            token = "";
            state = "FIND-TAG";
        } else if (state == "ESCAPE-CHARACTER") {
            while(c != ';'){
                infile.get(c);
            }
            token = "";
            state = "READ-TEXT";
        }

    }
    return 0;
}
