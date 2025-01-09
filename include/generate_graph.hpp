#ifndef GENERATE_GRAPH_HPP
#define GENERATE_GRAPH_HPP

#include "../include/graph.hpp"

void generate_graph(Graph &graph, vector<Node *> &coords, int R);
void generate_label_based_graph(Graph &graph, const vector<vector<double>> &coords);
void generate_random_edges(Graph &graph, int maxEdgesPerNode);

#endif