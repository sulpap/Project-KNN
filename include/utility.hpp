#ifndef UTILITY_HPP
#define UTILITY_HPP

#include "graph.hpp"
#include <vector>

using namespace std;

void read_fvecs(const char* filename);
double euclideanDistance(vector<double> coords1, vector<double> coords2);
double calculate_eclideanDistance(Node* node1, Node* node2);

#endif