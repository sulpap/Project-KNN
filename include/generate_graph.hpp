#ifndef GENERATE_GRAPH_HPP
#define GENERATE_GRAPH_HPP

#include "../include/graph.hpp"

void generate_graph(Graph &graph, vector<vector<double>> &coords, int R);
void generate_label_based_graph(Graph &graph, vector<vector<double>> &coords, const set<int> &F);

#endif