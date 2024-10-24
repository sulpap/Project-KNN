#ifndef UTILITY_HPP
#define UTILITY_HPP

#include "graph.hpp"
#include <vector>

using namespace std;

double euclidean_distance(vector<double> coords1, vector<double> coords2);
double euclidean_distance_of_nodes(Node* node1, Node* node2);
int findMedoid(const vector<vector<double>>& coords);

#endif