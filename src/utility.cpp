#include <../include/utility.hpp>
#include "../include/graph.hpp"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <algorithm>
#include <limits>

double euclidean_distance(vector<double> coords1, vector<double> coords2) 
{
    double sum = 0.0;
    double dist = 0.0;

    for (size_t i = 0; i < coords1.size(); ++i) {    
        dist = coords2[i] - coords1[i];    
        sum += dist * dist;
    }

    return sqrt(sum);
}

double euclidean_distance_of_nodes(Node* node1, Node* node2)
{
    //take the coords from the two nodes and call euclidean_distance
    return euclidean_distance(node1->getCoordinates(), node2->getCoordinates());
}

// double distance(vector<double> coords1, vector<double> coords2) 
// {
//     double sum = 0.0;
//     double dist = 0.0;

//     for (int i = 0; i < coords1.size(); ++i) {
//         dist = coords2[i] - coords1[i];    
//         sum += dist * dist;
//     }

//     return sqrt(sum);
// }

// Medoid is a representative object of a data set whose sum of distances to all the objects in the data set is minimal. 

// vrs. 1: classic method - takes 10 minutes for just the small base (10000 vectors)
// int findMedoid(const vector<vector<double>> &coords) 
// {
//     int medoidIndex = -1;
//     double minTotalDistance = numeric_limits<double>::max();

//     // for each point in each coord
//     for (size_t i = 0; i < coords.size(); ++i) 
//     {
//         double totalDistance = 0.0;

//         // calculate the sum of distances from the point to every other point
//         for (size_t j = 0; j < coords.size(); ++j) {
//             if (i != j) {
//                 totalDistance += euclidean_distance(coords[i], coords[j]);
//             }
//         }

//         // update medoid only if the sum is less than the minimum (until now)
//         if (totalDistance < minTotalDistance) {
//             minTotalDistance = totalDistance;
//             medoidIndex = i;
//         }
//     }

//     return medoidIndex;
// }

// vrs. 2 - better -> does half the computations - distance i->j and j->i is the same, so it's computed only once.
//                                                 also, we don't compute distances we have already computed. ex. for i = 2, j doesn't start from 0, 
//                                                 but from i + 1 = 3, because [2][0] and [2][1] have already been computed for i = 0 and i = 1.
int findMedoid(const vector<vector<double>>& coords) 
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
            if (i != j) {
                totalDistance += distance_matrix[i][j];
                // early exit if totalDistance exceeds current minimum --> avoid unnecessary calculations
                if (totalDistance >= minTotalDistance) break;
            }
        }

        // update medoid if we find a new minimum
        if (totalDistance < minTotalDistance) {
            minTotalDistance = totalDistance;
            medoidIndex = i;
        }
    }

    return medoidIndex;
}

vector<vector<double>> convert_to_double(const vector<vector<float>>& float_vector) {
    vector<vector<double>> double_vector(float_vector.size());
    for (size_t i = 0; i < float_vector.size(); i++) {
        double_vector[i] = vector<double>(float_vector[i].begin(), float_vector[i].end());
    }
    return double_vector;
}