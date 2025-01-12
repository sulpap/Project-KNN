#include <vector>
#include "../include/graph.hpp"

int Vamana_Generate_Graph_Parallel(Graph &graph, vector<Node *> &coords, int R, double a, int int_L); // added parameter f = label, to pass to generate_graph
                                                                              // so that it creates a graph with nodes of label f.
