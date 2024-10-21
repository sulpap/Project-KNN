#include "../include/greedysearch.hpp"
#include "../include/utility.hpp"
#include <algorithm>

// start point
// se kathe vhma koitaw geitones
// ypologizw apostash apo query
// epilegw closest geitona
// ksana

// GreedySearch returns node IDs !! not coords. so it's type vector<int> not vector<double>

// we start from the startNode and move through the graph to find the closest nodes to the query.

//if we have many unconnected graphs, we use graphUnion to combine them and then we do GreedySearch.

vector<int> GreedySearch(Graph &graph, int startNodeId, vector<double> &queryCoords, int k) 
{
    cout << "Initiating GreedySearch..." <<endl; //debugging

    vector<int> visitedNodes;
    vector<int> result;

    Node* currentNode = graph.getNode(startNodeId);

    if (!currentNode) {
        cout << "Start node does not exist in the graph." << endl;
        return result;
    }

    visitedNodes.push_back(currentNode->getId());

    // claculate the distance of our current node (startNode) and the query and store it
    double closestDistance = euclidean_distance(currentNode->getCoordinates(), queryCoords);
    int closestNodeId = currentNode->getId();


    // compare distances to the closestDistance in order to find the k closest nodes
    












    return result;
}