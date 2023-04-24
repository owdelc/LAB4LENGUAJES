#include "regex_utils.h"
#include "structs.h"
#pragma once

struct yalex_spec
{
    std::vector<std::string> re;
    std::unordered_map<std::string, std::string> accept_to_action;
};
yalex_spec yalex(std::string*);
std::vector<std::string> REGEX(std::string*);
void skip_ws_comments(std::string* input);