#ifndef STITCHEDVAMANA_HPP
#define STITCHEDVAMANA_HPP

#include "graph.hpp"
#include <unordered_map> // least complexity (less than map)

Graph stitchedVamana_Medoid_Subset(vector<vector<double>> &coords, set<int> F, double a, int L_small, int R_small, int R_stitched, map<int, Node *> &medoids);
unordered_map<int, set<int>> compute_Fx_medoid_subset(vector<vector<double>> &coords);
unordered_map<int, vector<Node *>> compute_PfMap_medoid_subset(vector<vector<double>> &coords, set<int> F);
void store_medoid_medoid_subset(Graph &G, map<int, Node *> &medoids, int f, int medoidId);

#endif