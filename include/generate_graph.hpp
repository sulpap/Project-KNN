#ifndef GENERATE_GRAPH_HPP
#define GENERATE_GRAPH_HPP

#include <unordered_map>
#include "../include/graph.hpp"

#define OFFSET 1000000 // so ids won't overlap. set to current possible maximum number of nodes.

void generate_graph(Graph &graph, vector<vector<double>> &coords, int R, int f, unordered_map<int, int> &indexes);
void generate_label_based_graph(Graph &graph, vector<vector<double>> &coords, const set<int> &F);

#endif