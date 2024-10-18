#ifndef UTILITY_HPP
#define UTILITY_HPP

#include "graph.hpp"
#include <vector>

using namespace std;

double euclidean_distance(vector<double> coords1, vector<double> coords2);
double euclidean_distance_of_nodes(Node* node1, Node* node2);
void calculate_distances(int nodeId, Graph& graph, vector<pair<double, int>>& distances);
void calculate_knn(int nodeId, Graph& graph, int k, vector<pair<double, int>>& knn);

#endif