#include <vector>
#include <string>
#include <unordered_map>
#include <set>
#pragma once



typedef long state;


struct node_direct
{
    bool leaf = false;
    int regexPosition;
    bool nullable;
    std::vector<int> first_pos;
    std::vector<int> last_pos;
    std::string character;
    node_direct* leftson;
    node_direct* rightson;
};

typedef std::vector<int> (*dir_func) (node_direct*, node_direct*, node_direct*);
typedef bool (*null_func) (node_direct*, node_direct*, node_direct*);

struct op_direct
{
    null_func nullable;
    dir_func first_pos;
    dir_func last_pos;
    int priority = 100000;
    std::string literal;
    bool unary = false;
    int regexPosition = -1;
};


typedef std::unordered_map<int, std::unordered_map<std::string, int>> Transition_table;




struct DFA
{
    int start;
    Transition_table trans;
    std::unordered_map<int, std::string> accept;
    std::set<std::string> alphabet;
    std::set<int> states;
    
};


//typedef std::unordered_map<std::pair<std::set<int>, std::string>, std::set<int>> Transition_table2;
namespace std
{
    template<> 
    struct hash<std::pair<std::set<int>, std::string>>{
    size_t operator()(const std::pair<std::set<int>, std::string> &s) {
    long long sum = s.second.c_str()[0];
    for ( auto e : s.first)
        sum += e;
    return sum;
    };
    };

    template<> 
    struct hash<std::set<int>>{
    size_t operator()(const std::set<int> &s) {
    long long sum = 0;
    for ( auto e : s)
        sum += e;
    return sum;
    };
    };
}





