#include "structs.h"
#include "direct_funcs.h"
using namespace std;


bool leaf_null(node_direct* me, node_direct* a, node_direct* b)
{
    if (me->character == "\0")
        return true;
    return false;
}

vector<int> leaf_first(node_direct* me, node_direct* a, node_direct* b)
{
    static int first_pos_counter = 0;
    vector<int> out;
    out.push_back(first_pos_counter++);
    return out;
}

vector<int> leaf_last(node_direct* me, node_direct* a, node_direct* b)
{
    static int last_pos_counter = 0;
    vector<int> out;
    out.push_back(last_pos_counter++);
    return out;
}


bool or_null(node_direct* me, node_direct* a, node_direct* b)
{
    return a->nullable || b->nullable;
}

std::vector<int> or_first(node_direct* me, node_direct* a, node_direct* b)
{
    vector<int> out;
    for (auto& ele:a->first_pos)
        out.push_back(ele);
    for (auto& ele:b->first_pos)
        out.push_back(ele);
    return out;
}

std::vector<int> or_last(node_direct* me, node_direct* a, node_direct* b)
{
    vector<int> out;
    for (auto& ele:a->last_pos)
        out.push_back(ele);
    for (auto& ele:b->last_pos)
        out.push_back(ele);
    return out;
}

bool qmark_null(node_direct* me, node_direct* a, node_direct* b)
{
    return true;
}

std::vector<int> qmark_first(node_direct* me, node_direct* a, node_direct* b)
{
    return a->first_pos;
}

std::vector<int> qmark_last(node_direct* me, node_direct* a, node_direct* b)
{
    return a->last_pos;
}

bool positive_null(node_direct* me, node_direct* a, node_direct* b)
{
    return a->nullable;
}

std::vector<int> positive_first(node_direct* me, node_direct* a, node_direct* b)
{
    return a->first_pos;
}

std::vector<int> positive_last(node_direct* me, node_direct* a, node_direct* b)
{
    return a->last_pos;
}

bool kleene_null(node_direct* me, node_direct* a, node_direct* b)
{
    return true;
}

std::vector<int> kleene_first(node_direct* me, node_direct* a, node_direct* b)
{
    return a->first_pos;
}

std::vector<int> kleene_last(node_direct* me, node_direct* a, node_direct* b)
{
    return a->last_pos;
}

bool concat_null(node_direct* me, node_direct* a, node_direct* b)
{
    return a->nullable && b->nullable;
}


std::vector<int> concat_first(node_direct* me, node_direct* a, node_direct* b)
{
    vector<int> out;
    for (auto& ele:a->first_pos)
                out.push_back(ele);
    if (a->nullable)
        for (auto& ele:b->first_pos)
                out.push_back(ele);
    return out;
}

std::vector<int> concat_last(node_direct* me, node_direct* a, node_direct* b)
{
    vector<int> out;
    
    if (b->nullable)
    {
        for (auto& ele:a->last_pos)
            out.push_back(ele);
        for (auto& ele:b->last_pos)
            out.push_back(ele);
        return out;
    }
    for (auto& ele:b->last_pos)
        out.push_back(ele);
    return out;
}