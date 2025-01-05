#ifndef GENERATE_GRAPH_HPP
#define GENERATE_GRAPH_HPP

#include <unordered_map>
#include "../include/graph.hpp"

#define OFFSET 1000000 // so ids won't overlap. set to current possible maximum number of nodes.

void generate_graph(Graph &graph, vector<Node *> &coords, int R);
void generate_label_based_graph(Graph &graph, const vector<vector<double>> &coords);

#endif