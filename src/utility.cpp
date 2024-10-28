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

    for (auto i = 0.0; i < coords1.size(); ++i) {    
        sum += pow(coords2[i] - coords1[i], 2);
    }

    return sqrt(sum);
}

double euclidean_distance_of_nodes(Node* node1, Node* node2)
{
    //take the coords from the two nodes and call euclidean_distance
    return euclidean_distance(node1->getCoordinates(), node2->getCoordinates());
}

// Medoids are representative objects of a data set 
// dissimilarities = distances to all the objects in the data set is minimal. 
int findMedoid(const vector<vector<double>> &coords) 
{
    int medoidIndex = -1;
    double minTotalDistance = numeric_limits<double>::max();

    // for each point in each coord
    for (size_t i = 0; i < coords.size(); ++i) 
    {
        double totalDistance = 0.0;

        // calculate the sum of distances from the point to every other point
        for (size_t j = 0; j < coords.size(); ++j) {
            if (i != j) {
                totalDistance += euclidean_distance(coords[i], coords[j]);
            }
        }

        // update medoid only if the sum is less than the minimum (until now)
        if (totalDistance < minTotalDistance) {
            minTotalDistance = totalDistance;
            medoidIndex = i;
        }
    }

    return medoidIndex;
}