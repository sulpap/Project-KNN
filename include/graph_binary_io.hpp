#ifndef GRAPH_BINARY_IO_HPP
#define GRAPH_BINARY_IO_HPP

#include "graph.hpp"

void save_graph_to_binary(const Graph &graph, const string &filename);
Graph load_graph_from_binary(const string &filename);
void save_map_to_binary(const map<int, Node *> &M, const string &filename);
map<int, Node *> load_map_from_binary(const string &filename, const Graph &graph);

#endif