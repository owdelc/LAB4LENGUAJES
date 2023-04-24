#include "yalex_val.h"
#include "direct.h"
#include "evaluate.h"
#include <iostream>
#include <stdexcept>
#include "dfa_utils.h"

using namespace std;


static DFA* let;
static DFA* ID;
static DFA* equals;
static DFA* SUBRE;
static DFA* action;
static DFA* rule;
static DFA* orop;
static DFA* LPAREN;
static DFA* RPAREN;
static DFA* closures;
static DFA* singleQuotes;
static DFA* charset_detail;
static vector<string> universe_regex;



static unordered_map<string, vector<string>> let_table;

static void yalex_init()
{
    ID = import_DFA("res/ID.dfa");

    let = import_DFA("res/LET.dfa");

    equals = import_DFA("res/EQUALS.dfa");

    SUBRE = import_DFA("res/SUBRE.dfa");

    rule = import_DFA("res/RULE.dfa");

    orop = import_DFA("res/OROP.dfa");

    LPAREN = import_DFA("res/LPAREN.dfa");

    RPAREN = import_DFA("res/RPAREN.dfa");

    closures = import_DFA("res/CLOSURES.dfa");

    singleQuotes = import_DFA("res/SINGLE.dfa");

    charset_detail = import_DFA("res/CHARSET_DETAIL.dfa");

    action = import_DFA("res/ACTION.dfa");

    universe_regex = get_universe_regex();
}

void skip_ws_comments(string* input)
{
    bool keep_looking = true;
    while(keep_looking)
    {
        keep_looking = false;
        while (!input->empty() && (*input)[0] < 33)
            input->erase(0, 1);
        if (input->size() > 2 && input->substr(0, 2) == "(*")
        {
            keep_looking = true;
            
            while (input->substr(0, 2) != "*)")
                input->erase(0, 1);
            
            input->erase(0, 2);
        }
    }
}

void skip_ws(string* input)
{
    while (!input->empty() && ((*input)[0] == 32 || (*input)[0] == 9))
        input->erase(0, 1);
}

static string plsEscape = "()*+?|\\.#";
static unordered_map<string, string> yalexEscape = {{"'\\n'", "\n"},
                                                    {"'\\t'", "\t"},
                                                    {"'\\\"'", "\""},
                                                    {"'\\\''", "\'"},
                                                    {"'\\v'", "\v"},
                                                    {"'\\r'", "\r"},
                                                    {"'\\a'", "\a"},
                                                    {"'\\b'", "\b"},
                                                    {"'\\f'", "\f"},
                                                    {"'\\s'", " "},
                                                    {"\\n", "\n"},
                                                    {"\\t", "\t"},
                                                    {"\\\'", "\""},
                                                    {"\\\'", "\'"},
                                                    {"\\v", "\v"},
                                                    {"\\r", "\r"},
                                                    {"\\a", "\a"},
                                                    {"\\b", "\b"},
                                                    {"\\f", "\f"},
                                                    {"\\s", " "}};


string single(string terminal)
{
    if (yalexEscape.find(terminal) != yalexEscape.end())
        return yalexEscape[terminal];
    string out;
    if (plsEscape.find(terminal[1]) != string::npos)
        out+="\\";
    out+=terminal[1];
    return out;
}

vector<string> double_func(string input)
{
    vector<string> out;
    input.erase(0, 1); //descartar "
    string temp;
    while(!input.empty() && input[0] != '"')
    {
        temp = input.substr(0, 2);
        if (yalexEscape.find(temp) != yalexEscape.end())
        {
            out.push_back(yalexEscape[temp]);
            input.erase(0, 2);
            if (input.size() > 1)
                out.push_back("|");
            continue;
        }
        temp = "";
        if (plsEscape.find(input[0]) != string::npos)
            temp+="\\";
        temp+=input[0];
        out.push_back(temp);
        input.erase(0, 1);
        if (input.size() > 1)
            out.push_back("|");
    }
    return out;
}

vector<string> double_regex(string input)
{
    vector<string> out;

    string insertion;
    for(int i = 1; i < input.size()-1; i++) 
    {
        insertion = "";
        insertion+=input[i];
        out.push_back(insertion);
        if (i+1 < input.size()-1)
            out.push_back(".");
    }

    return out;
}

vector<string> range_func(string input)
{
    vector<string> out;
    vector<token_info> res = get_first(singleQuotes, input);

    input.erase(0, res[0].pos+1);
    int start = res[0].content[1];
    res = get_first(singleQuotes, input);
    int end = res[0].content[1];

    if (start > end)
    {
        int temp = end;
        end = start;
        start = temp;
    }
    
    out.push_back("(");
    for(int i = start; i <= end; i++)
    {
        string insert;
        if (plsEscape.find((char)i) != string::npos)
            insert+="\\";
        insert+=(char)i;
        out.push_back(insert);
        if (i+1 <= end)
            out.push_back("|");
    }
    out.push_back(")");
    return out;
}

vector<string> charset(string input)
{
    vector<string> out;
    input.erase(0, 1);
    vector<token_info> res;
    out.push_back("(");


    if (input[0] == '^')
    {
        input.erase(0, 1);
        set<string> exclude = {"(", ")"};
        res = get_first(charset_detail, input);
        while (!res.empty())
        {
            input.erase(0, res[0].pos);
            if (res[0].called == "#range")
                for (auto&i:range_func(res[0].content))
                    if (i != "|")
                        exclude.insert(i);
            if (res[0].called == "#single")
                exclude.insert(single(res[0].content));
            if (res[0].called == "#double")
                for (auto&i:double_func(res[0].content))
                    if (i != "|")
                        exclude.insert(i);
            res = get_first(charset_detail, input);
        }
        bool skip_next = false;
        
        for(auto& i:universe_regex)
        {
            if (skip_next)
            {
                skip_next = false;
                continue;
            }
            if (exclude.find(i) != exclude.end())
            {
                skip_next = true;
                continue;
            }
            out.push_back(i);
        }
    }
    else
    {
        res = get_first(charset_detail, input);
        while (!res.empty())
        {
            input.erase(0, res[0].pos);
            if (res[0].called == "#range")
                for (auto&i:range_func(res[0].content))
                    out.push_back(i);
            if (res[0].called == "#single")
                out.push_back(single(res[0].content));
            if (res[0].called == "#double")
                for (auto&i:double_func(res[0].content))
                    out.push_back(i);
            res = get_first(charset_detail, input);
            if (!res.empty())
                out.push_back("|");
        }
    }
    out.push_back(")");
    return out;
}



vector<string> paren(string* input)
{
    vector<string> out;

    for(auto&i:REGEX(input))
        out.push_back(i);
    vector<token_info> res = get_first(RPAREN, *input);
    if (res.empty())
        throw invalid_argument("Unbalanced parenthesis");
    else input->erase(0, res[0].pos);
    out.push_back(")");
    return out;
}

vector<string> REGEX(string* input)
{
    vector<string> out;
    vector<token_info> res;

    res = get_first(SUBRE, *input);
    if (!res.empty())
    {
        input->erase(0, res[0].pos);
        if (res[0].called == "#charset")
            for(auto& i:charset(res[0].content))
                out.push_back(i);
        if (res[0].called == "#ID")
            if (let_table.find(res[0].content) == let_table.end())
                throw invalid_argument("Regex \"" + res[0].content + "\" is not defined.");
            else
                for(auto& i:let_table[res[0].content])
                    out.push_back(i);
        if (res[0].called == "#single")
            out.push_back(single(res[0].content));
        if (res[0].called == "#universe")
            for(auto&i:universe_regex)
                out.push_back(i);
        if (res[0].called == "#double")
            for(auto&i:double_regex(res[0].content))
                out.push_back(i);
    }
    
    bool keep_looking = true;
    
    while(keep_looking)
    {
        skip_ws(input);
        keep_looking = false;
        res = get_first(LPAREN, *input);
        if (!res.empty())
        {
            if (!out.empty())
                out.push_back(".");
            out.push_back("(");
            input->erase(0, res[0].pos);
            for(auto& i:paren(input))
                out.push_back(i);
            keep_looking = true;
        }
        
        res = get_first(closures, *input);
        while (!res.empty())
        {
            input->erase(0, res[0].pos);
            
            if (res[0].called == "#kleene")
                out.push_back("*");
            else if (res[0].called == "#positive")
                out.push_back("+");
            else if (res[0].called == "#questionMark")
                out.push_back("?");
            res = get_first(closures, *input);
            keep_looking = true;
        }
    }
    
    skip_ws(input);
    res = get_first(orop, *input);
    if (!res.empty())
    {
        input->erase(0, res[0].pos);
        out.push_back("|");
        for(auto&i:REGEX(input))
            out.push_back(i);
    }

    res = get_first(SUBRE, *input); 
    if (!res.empty()  && 
    !(res[0].called == "#ID" && res[0].content == "let"))
    {
        out.push_back(".");
        out.push_back("(");
        for(auto&i:REGEX(input))
            out.push_back(i);
        out.push_back(")");
    }

    return out;
}

yalex_spec action_func(string* input)
{
    static int counter;
    yalex_spec out;
    out.re.push_back("(");
    skip_ws_comments(input);
    

    for(auto&i:REGEX(input))
        out.re.push_back(i);

    skip_ws_comments(input);

    vector<token_info> res = get_first(action, *input);
    if (!res.empty())
    {
        input->erase(0, res[0].pos);
        out.re.push_back(".");
        out.re.push_back("#" + to_string(counter));
        out.accept_to_action[to_string(counter)] = res[0].content;
        counter++;
    }
    else 
    {
        out.re.push_back(".");
        out.re.push_back("#" + to_string(counter));
        out.accept_to_action[to_string(counter)] = ";"; 
        counter++;
    }

    skip_ws_comments(input);

    res = get_first(orop, *input);
    if (!res.empty())
    {
        out.re.push_back("|");
        input->erase(0, res[0].pos);
        yalex_spec next_action = action_func(input);
        for(auto&i:next_action.re)
            out.re.push_back(i);
        for(auto&i:next_action.accept_to_action)
            out.accept_to_action[i.first] = i.second;
    }
    out.re.push_back(")");
    return out;
}

yalex_spec ruledef(string* input)
{
    vector<token_info> res;

    string rule_id;
    res = get_first(ID, *input);
    if (!res.empty())
    {
        input->erase(0, res[0].pos);
        skip_ws_comments(input);
        rule_id = res[0].content;
    }
    else throw invalid_argument("Expected ID");

    res = get_first(equals, *input);
    if (!res.empty())
        input->erase(0, res[0].pos);
    
    else throw invalid_argument("Expected token \"=\"");

    return action_func(input);
}

void asig(string* input)
{
    vector<token_info> res;
    res = get_first(ID, *input);
    string found_ID;
    if (!res.empty())
    {
        input->erase(0, res[0].pos);
        found_ID = res[0].content;
        skip_ws_comments(input);
    }
    else throw invalid_argument("Expected ID");
    res = get_first(equals, *input);
    if (!res.empty())
    {
        input->erase(0, res[0].pos);
        skip_ws_comments(input);
    }
    else throw invalid_argument("Expected token \"=\"");
    vector<string> re = REGEX(input);
    let_table[found_ID] = re;

    
    skip_ws_comments(input);
}

yalex_spec yalex(string* input)
{
    static bool need_init = true;
    if(need_init)
    {
        yalex_init();
        need_init = false;
    }
    vector<token_info> res;
    skip_ws_comments(input);
    res = get_first(let, *input);
    while (!res.empty())
    {
        input->erase(0, res[0].pos);
        skip_ws_comments(input);
        asig(input);
        skip_ws_comments(input);
        res = get_first(let, *input);
    }

    cout << "\n\nDefinitions:\n";
    for (auto& foo:let_table)
    {
        cout << foo.first << ": ";
        for(auto&i:foo.second)
            cout << i;
        cout << "\n";
    }
    cout << endl;

    res = get_first(rule, *input);
    if (!res.empty())
    {
        input->erase(0, res[0].pos);
        skip_ws_comments(input);
        return ruledef(input);
    }
    else throw invalid_argument("No rule definition found");
    
}

