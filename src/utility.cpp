#include <../include/utility.hpp>
#include "../include/graph.hpp"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <cstdlib>  //
#include <ctime>  //

double euclidean_distance(vector<double> coords1, vector<double> coords2) 
{
    double sum = 0.0;

    for (auto i = 0.0; i < coords1.size(); ++i) {                           // TODO auto or smth else?     
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

// void calculate_knn --> keep the k nearest neighbors
void calculate_knn(int nodeId, Graph& graph, int k, vector<pair<double, int>>& knn) 
{
    vector<pair<double, int>> distances;

    calculate_distances(nodeId, graph, distances);

    //sort the distances to find the k nearest neighbors
    sort(distances.begin(), distances.end());

    // keep the k nearest neighbors:

    // if k > distances calculated then we iterate as many times as there are distances
    size_t times = min(k, static_cast<int>(distances.size()));

    for (size_t i = 0; i < times; i++) {
        knn.push_back(distances[i]);
    }
}

void generate_graph(Graph &graph, vector<vector<double>> &coords, int maxNodeEdges)
{
    srand(time(0));

    // nodeIds start from 1
    for (size_t i = 1; i <= coords.size(); i++) 
    {
        Node* newNode = new Node(i, coords[i - 1], {}); // vector coords start from 0
        graph.addNode(newNode);
    }

    //add random edges between the nodes
    for (size_t i = 1; i <= coords.size(); ++i) 
    {
        Node* current = graph.getNode(i);

        // random number of edges. at least 1 per node.
        int edgesToAdd = rand() % maxNodeEdges + 1;

        for (int j = 0; j < edgesToAdd; ++j) 
        {
            // random destination node
            size_t randomNeighborId = rand() % coords.size() + 1;

            //we don't add an edge to the same node or if the edge already exists
            if (randomNeighborId != i && !current->edgeExists(randomNeighborId)) {
                graph.addEdge(i, randomNeighborId);
            }
        }
    }

}