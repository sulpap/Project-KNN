#ifndef UTILITY_HPP
#define UTILITY_HPP

#include "graph.hpp"
#include <vector>
#include <unordered_map>

using namespace std;

double euclidean_distance(vector<double> coords1, vector<double> coords2);
double euclidean_distance_floats(vector<float> &coords1, vector<float> &coords2);
double euclidean_distance_of_nodes(Node *node1, Node *node2);
int findMedoid(const vector<vector<double>> &coords);
vector<vector<double>> convert_to_double(const vector<vector<float>> &float_vector);
void initialize_graph(Graph &G, const vector<vector<double>> &coords);
unordered_map<int, set<int>> computeFx(Graph &graph);

#endif