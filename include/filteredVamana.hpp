#ifndef FILTEREDVAMANA_HPP
#define FILTEREDVAMANA_HPP

#include "graph.hpp"

// we need the medoids after, so st_f is added to the parameters.
Graph filteredVamana(vector<vector<double>> &coords, double a, int int_L, int R,  set<int> F, int taph, map<int, Node *>& st_f);

#endif