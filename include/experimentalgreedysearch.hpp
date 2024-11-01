#include <vector>
#include "../include/graph.hpp"

using namespace std;

void ExperimentalGreedySearch(Graph &graph, Node* start_node, vector<double> &queryCoords, int k, int L, set<Node*> &L_set, set<Node*> &V_set);