#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>
#include "structs.h"
#include <sstream>
#include "yalex_val.h"
#include "shunting-yard.h"
#include <stdexcept>
#include "dfa_utils.h"
#include "direct.h"
#include "evaluate.h"
#include "regex_utils.h"

using namespace std;


int main(int argc, char* argv[])
{
    string srcPath = "input";
    string outPath = "scanner.cpp";
    string inputString = "";

    vector<string> args;
    for (int i = 1; i < argc; i++)
        args.push_back(argv[i]);

    bool use_out_file = false;

    for (int i = 0; i < args.size(); i++)
    {
        if (i+1 < args.size())
        {
            if (args[i] == "-f"
                && args[i+1] != "-t"
                && args[i+1] != "-ts"
                && args[i+1] != "-o"
                && args[i+1] != "-c")
                srcPath = args[++i];
            
            if (args[i] == "-o"
                && args[i+1] != "-t"
                && args[i+1] != "-ts"
                && args[i+1] != "-f"
                && args[i+1] != "-c")
            {
                    outPath = args[++i];
                    use_out_file = true;
            }
        }
    }

    ifstream srcfile = ifstream(srcPath);
    stringstream buffer;

    
    buffer << srcfile.rdbuf();
    string input = buffer.str();

    cout << input << "\n";

    stringstream out;


    DFA* action = import_DFA("res/ACTION.dfa");


    ifstream before_header = ifstream("res/before_header");
    out << before_header.rdbuf();
    before_header.close();

    skip_ws_comments(&input);
    vector<token_info> res = get_first(action, input);
    if (!res.empty())
    {
        input.erase(0, res[0].pos);
        cout << "header:\n" << res[0].content << endl; 
        out << "\n";
        out << res[0].content.substr(1, res[0].content.size()-2);
        out << "\n";
    }

    ifstream after_header = ifstream("res/after_header");
    out << after_header.rdbuf();
    after_header.close();

    DFA* my_dfa;
    ofstream out_stream = ofstream(outPath, ios::out);
    ofstream out_dfa_stream = ofstream("temp/temp.dfa", ios::out);
    try
    {
        yalex_spec spec = yalex(&input);
        cout << "Resulting regex:\n\n";
        for(auto&i:spec.re)
            cout<<i;
        cout << endl;
        my_dfa = do_direct(spec.re);

        for(auto&i:spec.accept_to_action)
        {
            out<< "if(i.called==\""<<i.first<<"\")\n";
            out<<i.second<<"\n";
            cout << i.first << "\t" << i.second << endl;
        }

        export_DFA(my_dfa, out_dfa_stream);
    } catch(const invalid_argument& e)
    {
        cerr << e.what() << endl;
    }

    out<< "}\nreturn 0;\n}";
    
    skip_ws_comments(&input);
    res = get_first(action, input);
    if (!res.empty())
    {
        input.erase(0, res[0].pos);
        cout << "trailer:\n" << res[0].content << endl; 
        out << "\n";
        out << res[0].content.substr(1, res[0].content.size()-2);
        out << "\n";
    }

    out_stream<<out.rdbuf()->str();
    return 0;
}
