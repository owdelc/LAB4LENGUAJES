#include "structs.h"
#include <iostream>

void export_DFA(DFA* dfa, std::ostream& out_stream);
DFA* import_DFA(std::istream& in_stream);
DFA* import_DFA(const char* path);
//DFA* import_or_create_DFA(const char* path); TODO