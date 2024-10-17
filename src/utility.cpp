#include <../include/utility.hpp>
#include "../include/graph.hpp"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <cassert>

double euclidean_distance(vector<double> coords1, vector<double> coords2) 
{
    double sum = 0.0;

    for (auto i = 0.0; i < coords1.size(); ++i) {                                 // auto or smth else??
        sum += pow(coords2[i] - coords1[i], 2);
    }

    return sqrt(sum);
}

double euclidean_distance_of_nodes(Node* node1, Node* node2)
{
    //take the coords from the two nodes and call euclidean_distance
    return euclidean_distance(node1->getCoordinates(), node2->getCoordinates());
}

// function to calculate distances from a specific node to all other nodes in the graph
void calculate_distances(int nodeId, Graph& graph, vector<pair<double, int>>& distances) 
{
    //get the specific node from the graph
    Node* targetNode = graph.getNode(nodeId);
    
    // check if it exists
    if (!targetNode) {
        cout << "Node with ID " << nodeId << " does not exist in the graph." << endl;
        return; // TODO return or exit failure????????????????
    }

    //iterate through all nodes in the graph
    for (auto& pair : graph.getAdjList()) {
        Node* current = pair.second;
        if (current->getId() != nodeId) { //skip the target node itself
            double dist = euclidean_distance_of_nodes(targetNode, current); 
            distances.push_back({dist, current->getId()});
        }
    }
}