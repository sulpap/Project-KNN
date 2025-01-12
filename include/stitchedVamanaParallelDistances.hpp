#ifndef STITCHEDVAMANAPARALLELDISTANCES_HPP
#define STITCHEDVAMANAPARALLELDISTANCES_HPP

#include "graph.hpp"

Graph stitchedVamanaParallelDistances(vector<vector<double>> &coords, set<int> F, double a, int L_small, int R_small, int R_stitched, map<int, Node *> &medoids);

#endif