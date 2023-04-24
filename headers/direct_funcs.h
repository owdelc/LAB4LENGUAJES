#include "structs.h"
#pragma once



bool leaf_null(node_direct* me, node_direct* a, node_direct* b);

std::vector<int> leaf_first(node_direct* me, node_direct* a, node_direct* b);

std::vector<int> leaf_last(node_direct* me, node_direct* a, node_direct* b);


bool or_null(node_direct* me, node_direct* a, node_direct* b);

std::vector<int> or_first(node_direct* me, node_direct* a, node_direct* b);

std::vector<int> or_last(node_direct* me, node_direct* a, node_direct* b);


bool qmark_null(node_direct* me, node_direct* a, node_direct* b);

std::vector<int> qmark_first(node_direct* me, node_direct* a, node_direct* b);

std::vector<int> qmark_last(node_direct* me, node_direct* a, node_direct* b);



bool positive_null(node_direct* me, node_direct* a, node_direct* b);

std::vector<int> positive_first(node_direct* me, node_direct* a, node_direct* b);

std::vector<int> positive_last(node_direct* me, node_direct* a, node_direct* b);


bool kleene_null(node_direct* me, node_direct* a, node_direct* b);

std::vector<int> kleene_first(node_direct* me, node_direct* a, node_direct* b);

std::vector<int> kleene_last(node_direct* me, node_direct* a, node_direct* b);


bool concat_null(node_direct* me, node_direct* a, node_direct* b);

std::vector<int> concat_first(node_direct* me, node_direct* a, node_direct* b);

std::vector<int> concat_last(node_direct* me, node_direct* a, node_direct* b);