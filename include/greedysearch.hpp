#include <vector>
#include "../include/graph.hpp"

using namespace std;

// L_set, V_set should be empty when calling the function
// L_set, V_set will be full once function call terminated
void GreedySearch(Node* start_node, vector<double> &queryCoords, int k, int L, set<Node*> &L_set, set<Node*> &V_set);

// L_set, V_set should be empty when calling the function
// L_set, V_set will be full once function call terminated
// See first lines of implementation for better explanation of parameters
void FilteredGreedySearch(set<Node*> &S_set, vector<double> &queryCoords, int k, int L, set<Node*> &L_set, set<Node*> &V_set, set<int> &F_q_set);
