#ifndef FILTEREDGREEDYSEARCH_HPP
#define FILTEREDGREEDYSEARCH_HPP

#include "../include/graph.hpp"

// L_set, V_set should be empty when calling the function
// L_set, V_set will be full once function call terminated
// See first lines of implementation for better explanation of parameters
void FilteredGreedySearch(set<Node *> &S_set, vector<double> &queryCoords, int k, int L, set<Node *> &L_set, set<Node *> &V_set, set<int> &F_q_set);

#endif