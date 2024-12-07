#ifndef FILTEREDVAMANA_HPP
#define FILTEREDVAMANA_HPP

#include "graph.hpp"
#include <unordered_map> // least complexity (less than map)

// we need the medoids after, so st_f is added to the parameters.
Graph filteredVamana(vector<vector<double>> &coords, double a, int int_L, int R, set<int> F, int taph, map<int, Node *> &st_f);
void initialize_graph(Graph &G, const vector<vector<double>> &coords);
unordered_map<int, set<int>> compute_Fx(Graph &graph, set<int> F);

#endif