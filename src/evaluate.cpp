#include "evaluate.h"
#include "structs.h"
#include <set>
#include "evaluate.h"
#include<iostream>

using namespace std;

string eval(DFA* dfa, string input)
{
    int current = dfa->start;
    for (auto& i:input) 
    {
        string current_char;
        current_char += i;
        if(dfa->trans.find(current) == dfa->trans.end())
            return "";
        unordered_map<string, int> current_trans = dfa->trans[current];
        if(current_trans.find(current_char) == current_trans.end())
            return "";
        current = current_trans[current_char];
    }
    if (dfa->accept.find(current) != dfa->accept.end()) 
        return dfa->accept[current];
    return "";
}

vector<token_info> tokenize(DFA* dfa, string input)
{
    vector<token_info> out;
    int current = dfa->start;
    int counter = 0;
    string content;
    string biggest;
    string lastCalled;
    token_info entry;
    
    for (int i = 0; i < input.size(); i++) 
    {
        if (input[i] == 13)
            input[i] = 10;
        string current_char;
        current_char += input[i];
        content += input[i];
        if(dfa->trans.find(current) == dfa->trans.end())
        {
            content.clear();
            current = dfa->start;

            if (!biggest.empty())
            {
                entry.called = lastCalled;
                entry.content = biggest;
                entry.pos = counter;
                out.push_back(entry);
                biggest.clear();
                counter = 0;
                i--;
            }
        }
        unordered_map<string, int> current_trans = dfa->trans[current];
        if(current_trans.find(current_char) == current_trans.end())
        {
            content.clear();
            current = dfa->start;

            if (!biggest.empty())
            {
                entry.called = lastCalled;
                entry.content = biggest;
                entry.pos = counter;
                out.push_back(entry);
                biggest.clear();
                counter = 0;
                i--;
            }
        }
        current = current_trans[current_char];
        if (dfa->accept.find(current) != dfa->accept.end()) 
        {
            if (content.size() > biggest.size())
            {
                biggest = content;
                lastCalled = dfa->accept[current].substr(1, string::npos);
            }
        }
        counter++;
    }
    
    if (dfa->accept.find(current) != dfa->accept.end()) 
    {
        biggest = content;
        lastCalled = dfa->accept[current].substr(1, string::npos);
        entry.called = lastCalled;
        entry.content = biggest;
        entry.pos = counter;
        out.push_back(entry);
    }
    return out;
}


vector<token_info> get_first(DFA* dfa, string input)
{
    vector<token_info> out;
    int current = dfa->start;
    int counter = 0;
    string content;
    string biggest;
    string lastCalled;
    token_info entry;
    
    for (int i = 0; i < input.size(); i++) 
    {
        
        string current_char;
        current_char += input[i];


        if(dfa->trans.find(current) == dfa->trans.end())
        {
            content.clear();
            current = dfa->start;

            if (!biggest.empty())
            {
                entry.called = lastCalled;
                entry.content = biggest;
                entry.pos = counter;
                out.push_back(entry);
            }
                return out;
        }
        else
        {
            unordered_map<string, int> current_trans = dfa->trans[current];
            if(current_trans.find(current_char) == current_trans.end())
            {
                content.clear();
                current = dfa->start;

                if (!biggest.empty())
                {
                    entry.called = lastCalled;
                    entry.content = biggest;
                    entry.pos = counter;
                    out.push_back(entry);
                }
                    return out;
            }
            else
            {
                content += input[i];
                current = current_trans[current_char];
                if (dfa->accept.find(current) != dfa->accept.end()) 
                {
                    if (content.size() > biggest.size())
                    {
                        biggest = content;
                        lastCalled = dfa->accept[current];
                    }
                }
            }
            
        }
        counter++;
    }
    if (!biggest.empty())
    {
        entry.called = lastCalled;
        entry.content = biggest;
        entry.pos = counter;
        out.push_back(entry);
    }
    return out;
}


string LookAhead(DFA* dfa, string input, int* current_index)
{
    int current = dfa->start;
    int i = *current_index;
    while (i < input.size()) 
    {
        string current_char;
        current_char+= input[i];
        if(dfa->trans.find(current) == dfa->trans.end())
            return "";
        unordered_map<string, int> current_trans = dfa->trans[current];
        if(current_trans.find(current_char) == current_trans.end())
            return "";
        current = current_trans[current_char];
        if (dfa->accept.find(current) != dfa->accept.end()) 
        {
            return dfa->accept[current];
            *current_index = i;
        }
    }
    return "";
}