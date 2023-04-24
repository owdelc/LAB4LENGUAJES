#include <string>
#include <fstream>
#include "structs.h"
#pragma once

DFA* do_direct(std::vector<std::string> regex);
void dump_DFA(DFA*, std::ostream&);
