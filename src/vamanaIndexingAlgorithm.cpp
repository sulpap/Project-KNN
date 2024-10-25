#include "../include/vamanaIndexingAlgorithm.hpp"
#include "../include/greedysearch.hpp"
#include "../include/robustprune.hpp"
#include "../include/generate_graph.hpp"
#include "../include/utility.hpp"
#include <algorithm>
#include <iostream>

using namespace std;

// coords = P
// k
// maxNodesEdges = R

// L is the set of closest neighbors returned from Greedy
// V is the set of visited nodes returned from Greedy

// neighborSet = Nout
// neighborId = j
// queryNode = σ(i)             to node me id i?

// Medoids are representative objects of a data set 
// whose sum of dissimilarities = distances to all the objects in the data set is minimal. 

void Vamana(vector<vector<double>> &coords, int maxNodesEdges, int k, int a)
{
    Graph graph;

    generate_graph(graph, coords, maxNodesEdges);

    int num_nodes = graph.getNodeCount();

    int s = findMedoid(coords);

    // vector<Node*> visited;

    // should we do a random shuffle from 1 to n?? and traverse randomly????

    for (auto i = 1; i <= num_nodes; i++)
    {
        Node* queryNode = graph.getNode(i);

        vector<Node*> V;

        vector<double> queryCoords = queryNode->getCoordinates(); // is this correct???????????? thelei &query coords????       
        vector<int> L = GreedySearch(graph, s, queryCoords, k); // greedy returns closest and visited

        // Convert V to a set for RobustPrune  ------ given that robust needs a SET as input
        set<Node*> neighborSet(V.begin(), V.end());

        RobustPrune(graph, queryNode, neighborSet, a, maxNodesEdges);
        // RobustPrune(graph, queryNode->getId(), neighborSet, a, maxNodesEdges);

        // add the pruned neigbors as edges
        for (int neighborId : L) {  // this is stated sthn ekfwnisi alla oxi sto psevdokwdika?????
            graph.addEdge(i, neighborId);
        }

        // check degree of the node. If it exceeds R, apply RobustPrune again.
        if (graph.getNode(i)->getEdges().size() > maxNodesEdges) {
            //re-apply RobustPrune to ensure degree <= R
            RobustPrune(graph, queryNode, neighborSet, a, maxNodesEdges);
        } else {
            // add neighbors since the degree constraint is not violated
            for (Node* neighbor : neighborSet) {
                graph.addEdge(i, neighbor->getId());
            }
        }
    
    }

}