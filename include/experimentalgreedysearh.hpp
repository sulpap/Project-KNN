#include <vector>
#include "../include/graph.hpp"

using namespace std;

// L_set, V_set should be empty when calling the function
// L_set, V_set will be full once function call terminated
void GreedySearch(Graph& G, Node* s, vector<double>& x_q, int k, int L, set<Node*> &back_L_set, set<Node*> &V_set);