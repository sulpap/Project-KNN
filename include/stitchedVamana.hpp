#include "graph.hpp"
#include <unordered_map> // least complexity (less than map)

Graph stitchedVamana(vector<vector<double>> &coords, set<int> F, double a, int L_small, int R_small, int R_stitched, map<int, Node *> &medoids);
unordered_map<int, set<int>> compute_Fx(vector<vector<double>> &coords);
unordered_map<int, vector<Node *>> compute_PfMap(vector<vector<double>> &coords, set<int> F);
void store_medoid(Graph &G, map<int, Node *> &medoids, int f, int medoidId);