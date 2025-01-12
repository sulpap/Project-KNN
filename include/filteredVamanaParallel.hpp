#ifndef FILTEREDVAMANAPARALLEL_HPP
#define FILTEREDVAMANAPARALLEL_HPP

#include "graph.hpp"
#include <unordered_map> // least complexity (less than map)

// we need the medoids after, so st_f is added to the parameters.
Graph filteredVamanaParallel(vector<vector<double>> &coords, double a, int int_L, int R, set<int> F, int taph, map<int, Node *> &st_f);

#endif