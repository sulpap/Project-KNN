#ifndef GENERATE_GRAPH_HPP
#define GENERATE_GRAPH_HPP

#include "../include/graph.hpp"
#include <unordered_map>

void generate_graph(Graph &graph, vector<Node *> &coords, int R);
void generate_graph_parallel(Graph &graph, vector<Node *> &coords, int R);
void generate_label_based_graph(Graph &graph, const vector<vector<double>> &coords);
void generate_random_edges(Graph &graph, int maxEdgesPerNode);
void connect_subgraphs(Graph &globalGraph, unordered_map<int, vector<Node *>> &PfMap);

#endif