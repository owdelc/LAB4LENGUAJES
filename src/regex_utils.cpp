#include "regex_utils.h"


using namespace std;

static string plsEscape = "()*+?|\\.#";

vector<string> get_universe_regex()
{
    vector<string> out;
    out.push_back("(");
    string temp;
    temp += (char)0;
    out.push_back(temp);
    
    for (int i = 1; i <= 126; i++)
    {
        temp = "";
        out.push_back("|");
        if (plsEscape.find((char)i) != string::npos)
            temp += "\\";
        temp += (char)i;
        out.push_back(temp);
    }
    out.push_back(")");
    return out;
}


vector<string> get_ID_regex()
{
    vector<string> letter;
    
    letter.push_back("(");
    string temp;
    temp += 'A';
    letter.push_back(temp);
    for (int i = (int)'B'; i <= (int)'Z'; i++)
    { 
        temp = "";
        letter.push_back("|");
        temp += (char)i;
        letter.push_back(temp);
    }
    for (int i = (int)'a'; i <= (int)'z'; i++)
    { 
        temp = "";
        letter.push_back("|");
        temp += (char)i;
        letter.push_back(temp);
    }
    letter.push_back(")");


    vector<string> digit;
    
    digit.push_back("(");
    digit.push_back("0");
    for (int i = (int)'1'; i <= (int)'9'; i++)
    { 
        temp = "";
        digit.push_back("|");
        temp += (char)i;
        digit.push_back(temp);
    }
    digit.push_back(")");

    vector<string> out;
    out.push_back("(");

    for(auto& i:letter)
        out.push_back(i);
    out.push_back(".");
    out.push_back("(");
    out.push_back("(");

    for(auto& i:letter)
        out.push_back(i);

    out.push_back("|");

    for(auto& i:digit)
        out.push_back(i);

    out.push_back(")");
    out.push_back("*");
    out.push_back(")");

    out.push_back(")");
    out.push_back(".");
    out.push_back("#ID");

    return out;
}

vector<string> get_action_regex()
{
    vector<string> out;
    out.push_back("(");
    out.push_back("{");
    out.push_back(".");
    out.push_back("(");
    string temp;
    temp += (char)0;
    out.push_back(temp);
    
    for (int i = 1; i <= 126; i++)
    {
        if (i == '}' || i == '{') continue;
        temp = "";
        out.push_back("|");
        if (plsEscape.find((char)i) != string::npos)
            temp += "\\";
        temp += (char)i;
        out.push_back(temp);
    }
    out.push_back(")");
    out.push_back("*");
    out.push_back(".");
    out.push_back("}");
    out.push_back(")");
    out.push_back(".");
    out.push_back("#action");

    return out;
}

vector<string> get_double_terminal()
{
    vector<string> double_terminal;
    string temp;

    double_terminal.push_back("(");
    double_terminal.push_back("\"");
    double_terminal.push_back(".");
    double_terminal.push_back("(");
    temp += (char)32;
    double_terminal.push_back(temp);
    
    for (int i = 33; i <= 126; i++)
    {
        if (i == '\"') continue;
        double_terminal.push_back("|");
        temp = "";
        if (plsEscape.find((char)i) != string::npos)
            temp += "\\";
        temp += (char)i;        
        double_terminal.push_back(temp);
    }
    
    string escape_chars = "nt\\\'\"vrabfs";

    for(auto i:escape_chars)
    {
        temp = "";
        temp+=i;
        double_terminal.push_back("(");
        double_terminal.push_back("|");
        double_terminal.push_back("\\");
        double_terminal.push_back(".");
        double_terminal.push_back(temp);
        double_terminal.push_back(")");
    }


    double_terminal.push_back(")");
    double_terminal.push_back("+");

    double_terminal.push_back(".");
    double_terminal.push_back("\"");

    double_terminal.push_back(")");
    return double_terminal;
}

vector<string> get_single_terminal()
{
    vector<string> single_terminal;
    string temp;

    single_terminal.push_back("(");
    single_terminal.push_back("'");
    single_terminal.push_back(".");
    temp += (char)32;
    single_terminal.push_back(temp);
    single_terminal.push_back(".");
    single_terminal.push_back("'");
    
    for (int i = 33; i <= 126; i++)
    {
        single_terminal.push_back("|");
        single_terminal.push_back("'");
        single_terminal.push_back(".");
        temp = "";
        if (plsEscape.find((char)i) != string::npos)
            temp += "\\";
        temp += (char)i;        
        single_terminal.push_back(temp);
        single_terminal.push_back(".");
        single_terminal.push_back("'");
    }
    
    string escape_chars = "nt\\\'\"vrabfs";

    for(auto i:escape_chars)
    {
        temp = "";
        temp+=i;
        single_terminal.push_back("|");
        single_terminal.push_back("'");
        single_terminal.push_back(".");
        single_terminal.push_back("\\");
        single_terminal.push_back(".");
        single_terminal.push_back(temp);
        single_terminal.push_back(".");
        single_terminal.push_back("'");
    }

    single_terminal.push_back(")");
    return single_terminal;
}

vector<string> get_range_regex()
{
    vector<string> range;
    vector<string> single_terminal = get_single_terminal();
    range.push_back("(");
    for(auto&i:single_terminal)
        range.push_back(i);
    range.push_back(".");
    range.push_back("-");
    range.push_back(".");
    for(auto&i:single_terminal)
        range.push_back(i);
    range.push_back(")");

    return range;
}



vector<string> get_charset()
{
    vector<string> charset;
    vector<string> single_terminal = get_single_terminal();
    vector<string> range = get_range_regex();
    vector<string> double_terminal = get_double_terminal();

    charset.push_back("(");
    charset.push_back("[");
    charset.push_back(".");
    charset.push_back("^");
    charset.push_back("?");
    charset.push_back(".");
    charset.push_back("(");
    charset.push_back("(");
    for(auto&i:range)
        charset.push_back(i);

    charset.push_back("|");

    for(auto&i:single_terminal)
        charset.push_back(i);

    charset.push_back(")");
    charset.push_back("+");

    charset.push_back("|"); 
    charset.push_back("("); 
    for(auto&i:double_terminal)
        charset.push_back(i);
    charset.push_back(")"); 

    charset.push_back(")");

    charset.push_back(".");
    charset.push_back("]");
    charset.push_back(")");

    return charset;
}

vector<string> get_subre_regex()
{
    vector<string> single_terminal = get_single_terminal();
    vector<string> double_terminal = get_double_terminal();
    vector<string> charset = get_charset();
    vector<string> ID = get_ID_regex();
    vector<string> subre;

    subre.push_back("(");

    subre.push_back("(");

    for(auto&i:charset)
        subre.push_back(i);
    subre.push_back(".");
    subre.push_back("#charset");

    subre.push_back("|");

    for(auto&i:single_terminal)
        subre.push_back(i);
    subre.push_back(".");
    subre.push_back("#single");

    subre.push_back("|");

    for(auto&i:ID)
        subre.push_back(i);
    subre.push_back(".");
    subre.push_back("#ID");

    subre.push_back("|");

    subre.push_back("_");
    subre.push_back(".");
    subre.push_back("#universe");

    subre.push_back("|");

    subre.push_back("(");
    for(auto&i:double_terminal)
        subre.push_back(i);
    subre.push_back(".");
    subre.push_back("#double");
    subre.push_back(")");

    subre.push_back(")");

    subre.push_back("+");

    subre.push_back(")");

    return subre;
}

vector<string> get_let_regex()
{
    vector<string> let;

    let.push_back("(");
    let.push_back("l");
    let.push_back(".");
    let.push_back("e");
    let.push_back(".");
    let.push_back("t");
    let.push_back(")");
    let.push_back(".");
    let.push_back("#let");

    return let;
}

vector<string> get_equals_regex()
{
    vector<string> equals;

    equals.push_back("(");
    equals.push_back("=");
    equals.push_back(")");
    equals.push_back(".");
    equals.push_back("#equals");

    return equals;
}

vector<string> get_closures_regex()
{
    vector<string> closures;

    closures.push_back("(");

    closures.push_back("(");
    closures.push_back("\\*");
    closures.push_back(".");
    closures.push_back("#kleene");
    closures.push_back(")");

    closures.push_back("|");

    closures.push_back("(");
    closures.push_back("\\+");
    closures.push_back(".");
    closures.push_back("#positive");
    closures.push_back(")");

    closures.push_back("|");

    closures.push_back("(");
    closures.push_back("\\?");
    closures.push_back(".");
    closures.push_back("#questionMark");
    closures.push_back(")");

    closures.push_back(")");

    return closures;
}

vector<string> get_rule_regex()
{
    vector<string> rule;

    rule.push_back("(");
    rule.push_back("r");
    rule.push_back(".");
    rule.push_back("u");
    rule.push_back(".");
    rule.push_back("l");
    rule.push_back(".");
    rule.push_back("e");
    rule.push_back(")");
    rule.push_back(".");
    rule.push_back("#rule");

    return rule;
}

vector<string> get_orop_regex()
{
    vector<string> out;

    out.push_back("(");
    out.push_back("\\|");
    out.push_back(")");
    out.push_back(".");
    out.push_back("#OROP");

    return out;
}

vector<string> get_Lparen_regex()
{
    vector<string> out;

    out.push_back("(");
    out.push_back("\\(");
    out.push_back(")");
    out.push_back(".");
    out.push_back("#LPAREN");

    return out;
}
vector<string> get_Rparen_regex()
{
    vector<string> out;

    out.push_back("(");
    out.push_back("\\)");
    out.push_back(")");
    out.push_back(".");
    out.push_back("#RPAREN");

    return out;
}

vector<string> get_charset_detail()
{
    vector<string> out;
    vector<string> single_terminal = get_single_terminal();
    vector<string> range = get_range_regex();
    vector<string> double_terminal = get_double_terminal();

    out.push_back("(");
    out.push_back("(");

    for(auto&i:single_terminal)
        out.push_back(i);

    out.push_back(")");
    out.push_back(".");
    out.push_back("#single");

    out.push_back("|");

    out.push_back("(");

    for(auto&i:double_terminal)
        out.push_back(i);

    out.push_back(")");
    out.push_back(".");
    out.push_back("#double");


    out.push_back("|");

    out.push_back("(");

    for(auto&i:range)
        out.push_back(i);

    out.push_back(")");
    out.push_back(".");
    out.push_back("#range");

    out.push_back(")");

    return out;
}