#ifndef STITCHEDVAMANAPARALLEL_HPP
#define STITCHEDVAMANAPARALLEL_HPP

#include "graph.hpp"
#include <unordered_map> // least complexity (less than map)

Graph stitchedVamanaParallel(vector<vector<double>> &coords, set<int> F, double a, int L_small, int R_small, int R_stitched, map<int, Node *> &medoids);

#endif