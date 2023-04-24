#include "direct.h"
#include "shunting-yard.h"
#include <iostream>
#include <unordered_map>
#include <vector>
#include "direct_funcs.h"
#include <utility>
#include <algorithm>

using namespace std;

void postorderdirectprint(node_direct* root)
{
    if (root->leftson)
        postorderdirectprint(root->leftson);
    if (root->rightson)
        postorderdirectprint(root->rightson);
    cout << "{ ";
    for (auto& ele:root->first_pos)
        cout << ele << " ";
    cout << "} \t";
    cout << root->character << " \t{ ";
    for (auto& ele:root->last_pos)
        cout << ele << " ";
    cout << "} \n";
}


void followpos(node_direct* a, unordered_map<int, set<int>>* out)
{
    if (a->leftson)
        followpos(a->leftson, out);
    if (a->rightson)
        followpos(a->rightson, out);
    if (a->character == ".")
        for (auto& first:a->leftson->last_pos)
            for (auto& last:a->rightson->first_pos)
                (*out)[first].insert(last);
    
    if (a->character == "*" || a->character == "+")
        for (auto& last:a->last_pos)
            for (auto& first:a->first_pos)
                (*out)[last].insert(first);

    if (a->leaf)
        out->insert({*(a->first_pos.begin()), set<int>()});
}

static set<string> alphabet;
static unordered_map<int, string> pos_to_leaf;
static unordered_map<int, string> end_pos;
void map_pos_to_leaf(node_direct* root)
{
    if (root->leftson)
        map_pos_to_leaf(root->leftson);
    if (root->rightson)
        map_pos_to_leaf(root->rightson);
    if (root->leaf)
    {
        pos_to_leaf.insert({root->first_pos[0], root->character});
        alphabet.insert(root->character);
        if (root->character[0] == '#')
            end_pos.insert({root->first_pos[0], root->character});
    }
}

state getNewID()
{
    static state counter = 0;
    return counter++;
}


inline int set_to_state(set<int> a)
{
    static vector<pair<set<int>, int>> set_state_map;

    for(auto& i:set_state_map)
        if (i.first == a)
            return i.second;

    int new_id = getNewID();
    set_state_map.push_back({a, new_id});
    return new_id;
}


DFA* createDFA(node_direct* root, unordered_map<int, set<int>>& follow)
{
    const vector<string> regex_escape = {"\\*", "\\+", "\\?", "\\#", "\\)", "\\(", "\\.", "\\|", "\\\\"};
    DFA *out = new DFA();
    Transition_table trans;
    vector<set<int>> unmarked;

    set<int> start;
    for (auto& p:root->first_pos)
        start.insert(p);
    unmarked.push_back(start);

    out->start = set_to_state(start);

    set<set<int>> in_trans;
    in_trans.insert(unmarked.back());

    while (!unmarked.empty())
    {
        set<int> S = unmarked.back();
        unmarked.pop_back();
        in_trans.insert(S);
        out->states.insert(set_to_state(S));

        for (auto& p:S)
            if(end_pos.find(p) != end_pos.end())
            {
                out->accept.insert({set_to_state(S), end_pos[p]});
                break;
            }

        for (auto& character:alphabet)
        {
            set<int> U;
            for (auto& p:S) 
                if (pos_to_leaf[p] == character)
                    for (auto& f:follow[p])
                        U.insert(f);

            if (U.empty()) continue;

            if (in_trans.find(U) == in_trans.end())
                unmarked.push_back(U);
            
            if (trans.find(set_to_state(S)) == trans.end())
                trans.insert({set_to_state(S), unordered_map<string, int>()});
            
            if (find(regex_escape.begin(), regex_escape.end(), character) == regex_escape.end())
                trans[set_to_state(S)].insert({character, set_to_state(U)});
            else
                trans[set_to_state(S)].insert({character.substr(1, character.npos), set_to_state(U)});
            
        }
    }

    out->trans = trans;
    out->alphabet = alphabet;
    
    return  out;
}

void deltree(node_direct* root)
{
    if (root->leftson)
        deltree(root->leftson);
    if (root->rightson)
        deltree(root->rightson);
    delete root;
}


DFA* do_direct(vector<string> regex)
{    
    try
    {
    node_direct* finalTree  = dijkstra_direct(regex);

    unordered_map<int, set<int>> follow;
    followpos(finalTree, &follow);



    map_pos_to_leaf(finalTree);
    DFA *finalAutomata = createDFA(finalTree, follow);

    deltree(finalTree);
    alphabet.clear();

    return finalAutomata;
    } catch(const invalid_argument& e)
    {
        cerr << e.what() << endl;
    }
    return 0;
}

void dump_DFA(DFA* finalAutomata, std::ostream& out_stream)
{
    string header = "digraph finite_state_machine {\nfontname=%f\nnode [fontname=%f]\nedge [fontname=%f]\nrankdir=LR;\nnode [shape = doublecircle]; %a;\nnode [shape = point ]; qi\nnode [shape = circle];\n";
        size_t pos;
    while ((pos = header.find("%f")) != std::string::npos) 
        header.replace(pos, 2, "\"Helvetica,Arial,sans-serif\"");


    string accept_states = "";
    for (auto& s:finalAutomata->accept)
        accept_states+= to_string(s.first) + ",";

    accept_states.pop_back();
    while ((pos = header.find("%a")) != std::string::npos) 
        header.replace(pos, 2, accept_states);

    string out = header;

    for (auto& foo:finalAutomata->trans)
        for (auto& bruh:foo.second)
    {
        string insertion;
        insertion = to_string(foo.first);
        insertion += "->";
        insertion += to_string(bruh.second);
        insertion += " [label = \"";
        string formated_label = "";
        for(auto& ch:bruh.first)
            if (ch == '\"' || ch == '[' || ch == ']' || ch == '\\')
            {
                formated_label += '\\';
                formated_label += ch;
            }
            else
                if (ch > 32)
                    formated_label += ch;
                else
                    formated_label += "ASCII: " + to_string((int)ch);

        insertion += formated_label;
        insertion += "\"];\n";
        out+= insertion;
    }



    string startNodeMarker = "qi -> %s;";
    while ((pos = startNodeMarker.find("%s")) != std::string::npos) 
        startNodeMarker.replace(pos, 2, to_string(finalAutomata->start));
    
    out += startNodeMarker;

    out+="}";
    out_stream << out << endl;
}