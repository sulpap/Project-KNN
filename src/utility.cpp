#include <../include/utility.hpp>
#include "../include/graph.hpp"
#include "../include/greedysearch.hpp"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <cassert>
#include <algorithm>

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

// TODO should the index start from 1?
int findMedoid(const vector<vector<double>> &coords) 
{
    int medoidIndex = -1;
    double minTotalDistance = 10000000000000; // TODO maybe change this

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

        // Print the total distance for debugging
        cout << "Total distance for point " << i << ": " << totalDistance << endl;

        // update medoid only if the sum is less than the minimum (until now)
        if (totalDistance < minTotalDistance) {
            minTotalDistance = totalDistance;
            medoidIndex = i;
        }
    }

    return medoidIndex;
}



// POSSIBLY NOT NECESSARY
// function to calculate distances from a specific node to all other nodes in the graph
// void calculate_distances(int nodeId, Graph& graph, vector<pair<double, int>>& distances) 
// {
//     //get the specific node from the graph
//     Node* targetNode = graph.getNode(nodeId);
    
//     // check if it exists
//     if (!targetNode) {
//         cout << "Node with ID " << nodeId << " does not exist in the graph." << endl;
//         return; // TODO return or exit failure????????????????
//     }

//     //iterate through all nodes in the graph
//     for (auto& pair : graph.getAdjList()) {
//         Node* current = pair.second;
//         if (current->getId() != nodeId) { //skip the target node itself
//             double dist = euclidean_distance_of_nodes(targetNode, current); 
//             distances.push_back({dist, current->getId()});
//         }
//     }
// }




// void calculate_knn --> keep the k nearest neighbors
// void calculate_knn(int nodeId, Graph& graph, int k, vector<pair<double, int>>& knn) 
// {
//     vector<pair<double, int>> distances;

//     calculate_distances(nodeId, graph, distances);

//     //sort the distances to find the k nearest neighbors
//     sort(distances.begin(), distances.end());

//     // keep the k nearest neighbors:

//     // if k > distances calculated then we iterate as many times as there are distances
//     size_t times = min(k, static_cast<int>(distances.size()));

//     for (size_t i = 0; i < times; i++) {
//         knn.push_back(distances[i]);
//     }
// }


// POSSIBLY NOT NECESSARY
// uses greedysearch to take all the knn and stores their sitances
void calculate_knn(int nodeId, Graph& graph, int k, vector<pair<double, int>>& knn) 
{
    cout << "Initiating calculate_knn..." << endl; //debugging

    Node* startNode = graph.getNode(nodeId);

    if (!startNode) {
        cout << "Node with ID " << nodeId << " does not exist in the graph." << endl;
        return;
    }

    vector<double> startCoords = startNode->getCoordinates();
    vector<int> closestNodes = GreedySearch(graph, nodeId, startCoords, k);

    for (int neighborId : closestNodes) {
        if (neighborId != nodeId) {
            double dist = euclidean_distance_of_nodes(startNode, graph.getNode(neighborId));
            knn.push_back({dist, neighborId});
        }
    }

    sort(knn.begin(), knn.end());
}
