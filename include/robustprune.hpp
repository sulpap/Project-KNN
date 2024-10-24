#ifndef ROBUSTPRUNE_HPP
#define ROBUSTPRUNE_HPP

#include "../include/graph.hpp"

using namespace std;

void RobustPrune(Graph& G, Node* p, set<Node*> V, double a, int R);

#endif
#include "graph.hpp"

