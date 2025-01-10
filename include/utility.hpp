#ifndef UTILITY_HPP
#define UTILITY_HPP

#include "graph.hpp"
#include <vector>

using namespace std;

double euclidean_distance(vector<double> coords1, vector<double> coords2);
double euclidean_distance_floats(vector<float> &coords1, vector<float> &coords2);
double euclidean_distance_of_nodes(Node *node1, Node *node2);
int findMedoid(const vector<vector<double>> &coords);
int parallel_findMedoid(const vector<vector<double>> &coords);
int parallel_2_findMedoid(const vector<vector<double>> &coords);
int findMedoid_random(const vector<vector<double>> &coords);
int findMedoidInSubset(const vector<vector<double>> &coords);
vector<vector<double>> convert_to_double(const vector<vector<float>> &float_vector);

#endif