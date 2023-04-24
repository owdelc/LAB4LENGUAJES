#include "dfa_utils.h"
#include <iostream>
#include "direct.h"


using namespace std;

void export_DFA(DFA* dfa, std::ostream& out_stream)
{
    out_stream.write((char*) &dfa->start, sizeof(int));

    int marshall_mem;
    for (auto&i:dfa->accept)
    {
        out_stream.write((char*) &i.first, sizeof(int));
        marshall_mem = i.second.size();
        out_stream.write((char*) &marshall_mem, sizeof(int));
        out_stream.write(i.second.c_str(), i.second.size());
    }

    constexpr int end_of_accept = -1;
    out_stream.write((char*) &end_of_accept, sizeof(int));

    for(auto&i:dfa->trans)
        for(auto&j:i.second)
        {
            out_stream.write((char*) &i.first, sizeof(int));
            out_stream.write(j.first.c_str(), 1);
            out_stream.write((char*) &j.second, sizeof(int));
        }

    out_stream.write((char*) &end_of_accept, sizeof(int));
}


DFA* import_DFA(std::istream& in_stream)
{
    DFA* out = new DFA();
    in_stream.read((char*) &out->start, sizeof(int));

    int s;
    while(true)
    {
        in_stream.read((char*) &s, sizeof(int));

        if (s < 0)
            break;
        int cat_size;
        in_stream.read((char*) &cat_size, sizeof(int));

        char* buff = new char[cat_size+1];
        in_stream.read((char*) buff, cat_size);
        buff[cat_size] = 0;
        string cat(buff);

        out->accept.insert({s, cat});
        delete[] buff;
    }   
    
    while(true)
    {
        in_stream.read((char*) &s, sizeof(int));
        if(s < 0)
            break;
        
        char buff[1];
        in_stream.read((char*) buff, 1);
        string cat;
        cat+=buff[0];

        int next;
        in_stream.read((char*) &next, sizeof(int));

        if (out->trans.find(s) == out->trans.end())
            out->trans.insert({s, unordered_map<string, int>()});
        out->trans[s].insert({cat, next});
    }
    return out;
}

DFA* import_DFA(const char* path)
{
    ifstream in_stream = ifstream(path, ios::out | ios::binary);
    DFA* out = new DFA();
    in_stream.read((char*) &out->start, sizeof(int));

    int s;
    while(true)
    {
        in_stream.read((char*) &s, sizeof(int));

        if (s < 0)
            break;
        int cat_size;
        in_stream.read((char*) &cat_size, sizeof(int));

        char* buff = new char[cat_size];
        in_stream.read((char*) buff, cat_size);
        string cat(buff);

        out->accept.insert({s, cat});
        delete[] buff;
    }   
    
    while(true)
    {
        in_stream.read((char*) &s, sizeof(int));
        if(s < 0)
            break;
        
        char buff[1];
        in_stream.read((char*) buff, 1);
        string cat;
        cat+=buff[0];

        int next;
        in_stream.read((char*) &next, sizeof(int));

        if (out->trans.find(s) == out->trans.end())
            out->trans.insert({s, unordered_map<string, int>()});
        out->trans[s].insert({cat, next});
    }

    in_stream.close();
    
    return out;
}