#include <vector>
#include "../include/graph.hpp"

using namespace std;

// vector<int> GreedySearch(Graph &graph, int startNode, vector<double> &queryCoords, int k);       // Θεοδωσία's


// L_set, V_set should be empty when calling the function
// L_set, V_set will be full once function call terminated
void GreedySearch(Graph &graph, int startNodeId, vector<double> &queryCoords, int k, int L, set<int> &L_set, set<int> &V_set); 