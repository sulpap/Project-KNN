#ifndef GENERATE_GRAPH_HPP
#define GENERATE_GRAPH_HPP

#include <unordered_map>
#include "../include/graph.hpp"

#define OFFSET 10000

void generate_graph(Graph &graph, vector<vector<double>> &coords, int R, int f, unordered_map<int, int> &indexes);
void generate_label_based_graph(Graph &graph, vector<vector<double>> &coords, const set<int> &F);

#endif