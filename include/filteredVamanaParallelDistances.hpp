#ifndef FILTEREDVAMANAPARALLELDISTANCES_HPP
#define FILTEREDVAMANAPARALLELDISTANCES_HPP

#include "graph.hpp"
#include <unordered_map> // least complexity (less than map)

// we need the medoids after, so st_f is added to the parameters.
Graph filteredVamanaParallelDistances(vector<vector<double>> &coords, double a, int int_L, int R, set<int> F, int taph, map<int, Node *> &st_f);

/* there is no need to declare the other functions here. 
we declared them before to be able to write their unit tests.
they are the exact same as in the filteredVamana.cpp file, so 
they already have unit tests. Only thing changing is their name.
*/

#endif