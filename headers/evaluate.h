#include "structs.h"
#pragma once

std::string eval(DFA*, std::string);

struct token_info
{
    int pos;
    std::string called;
    std::string content;
};
std::vector<token_info> tokenize(DFA*, std::string);
std::vector<token_info> get_first(DFA*, std::string);