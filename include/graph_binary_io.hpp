#ifndef GRAPH_BINARY_IO_HPP
#define GRAPH_BINARY_IO_HPP

#include "graph.hpp"

void save_graph_to_binary(const Graph &graph, const string &filename);
Graph load_graph_from_binary(const string &filename);

#endif