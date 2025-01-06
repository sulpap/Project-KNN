#include <../include/utility.hpp>
#include "../include/graph.hpp"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <algorithm>
#include <limits>
#include <cassert>
#include <cstdlib>
#include <ctime>

double euclidean_distance(vector<double> coords1, vector<double> coords2)
{   
    assert(coords1.size() == coords2.size());
    double sum = 0.0;
    double dist = 0.0;

    for (size_t i = 0; i < coords1.size(); ++i)
    {
        dist = coords2[i] - coords1[i];
        sum += dist * dist;
    }

    return sqrt(sum);
}

// Οι παράμετροι της συνάρτησης έχουν μόνο τις διαστάσεις. Όχι τις ετικέτες ή query_type.
double euclidean_distance_floats(vector<float> &coords1, vector<float> &coords2)
{
    assert(coords1.size() == coords2.size());
    double sum = 0.0;
    double dist = 0.0;

    for (size_t i = 0; i < coords1.size(); i++)
    {
        dist = coords2[i] - coords1[i];
        sum += dist * dist;
    }

    return sqrt(sum);
}

double euclidean_distance_of_nodes(Node *node1, Node *node2)
{
    // take the coords from the two nodes and call euclidean_distance
    return euclidean_distance(node1->getCoordinates(), node2->getCoordinates());
}

// vrs. 2 - better -> does half the computations - distance i->j and j->i is the same, so it's computed only once.
//                                                 also, we don't compute distances we have already computed. ex. for i = 2, j doesn't start from 0,
//                                                 but from i + 1 = 3, because [2][0] and [2][1] have already been computed for i = 0 and i = 1.
int findMedoid(const vector<vector<double>> &coords)
{
    int medoidIndex = -1;
    double minTotalDistance = numeric_limits<double>::max();
    size_t n = coords.size();

    // precompute pair distances and store them
    vector<vector<double>> distance_matrix(n, vector<double>(n, 0.0)); // size n and initialized with 0.0 for each element
    for (size_t i = 0; i < n; ++i)
    {
        for (size_t j = i + 1; j < n; ++j)
        {
            double dist = euclidean_distance(coords[i], coords[j]);
            distance_matrix[i][j] = dist;
            distance_matrix[j][i] = dist;
        }
    }

    // find the medoid
    for (size_t i = 0; i < n; ++i)
    {
        double totalDistance = 0.0;
        for (size_t j = 0; j < n; ++j)
        {
            if (i != j)
            {
                totalDistance += distance_matrix[i][j];
                // early exit if totalDistance exceeds current minimum --> avoid unnecessary calculations
                if (totalDistance >= minTotalDistance)
                    break;
            }
        }

        // update medoid if we find a new minimum
        if (totalDistance < minTotalDistance)
        {
            minTotalDistance = totalDistance;
            medoidIndex = i;
        }
    }

    return medoidIndex;
}

int findMedoid_random(const vector<vector<double>> &coords)
{
    // Seed the random number generator to ensure different results on each run
    std::srand(static_cast<unsigned>(std::time(0)));

    // Select a random index from the coords vector
    int randomIndex = std::rand() % coords.size();

    return randomIndex;
}

int findMedoidInSubset(const std::vector<std::vector<double>> &coords, size_t subsetSize) {
    size_t n = coords.size();
    if (subsetSize > n) {
        subsetSize = n; // Ensure subsetSize does not exceed total points
    }

    // Generate a random subset of indices
    std::vector<int> indices(n);
    for (size_t i = 0; i < n; ++i) {
        indices[i] = i;
    }

    // Shuffle and select the first subsetSize indices
    std::srand(static_cast<unsigned>(std::time(0)));
    std::random_shuffle(indices.begin(), indices.end());
    std::vector<int> subset(indices.begin(), indices.begin() + subsetSize);

    // Compute the medoid within the subset
    int medoidIndex = -1;
    double minTotalDistance = std::numeric_limits<double>::max();

    for (int i : subset) {
        double totalDistance = 0.0;
        for (int j : subset) {
            if (i != j) {
                totalDistance += euclidean_distance(coords[i], coords[j]);
                if (totalDistance >= minTotalDistance) { // Early exit
                    break;
                }
            }
        }

        if (totalDistance < minTotalDistance) {
            minTotalDistance = totalDistance;
            medoidIndex = i;
        }
    }

    return medoidIndex;
}

vector<vector<double>> convert_to_double(const vector<vector<float>> &float_vector)
{
    vector<vector<double>> double_vector(float_vector.size());
    for (size_t i = 0; i < float_vector.size(); i++)
    {
        double_vector[i] = vector<double>(float_vector[i].begin(), float_vector[i].end());
    }
    return double_vector;
}