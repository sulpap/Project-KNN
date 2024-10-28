#include "../include/vamana.hpp"
#include "../include/greedysearch.hpp"
#include "../include/robustprune.hpp"
#include "../include/generate_graph.hpp"
#include "../include/utility.hpp"
#include <algorithm>
#include <iostream>

using namespace std;

// coords = coords of all vectors in P
// k
// maxNodesEdges = R
// randomPermutation = σ

// L is the set of closest neighbors returned from Greedy
// V is the set of visited nodes returned from Greedy
// Nout are the possible candidates, so the union of L and V.

// neighbor = j
// queryNode = σ(i)             to node me id i?

// Medoids are representative objects of a data set 
// whose sum of dissimilarities = distances to all the objects in the data set is minimal. 

void Vamana(vector<vector<double>> &coords, int maxNodesEdges, int k, int a)
{
    Graph graph;

    generate_graph(graph, coords);

    Node* s = graph.getNode(findMedoid(coords));

    // make a random permutation of 1..n, to traverse the nodes in a random order
    int num_nodes = graph.getNodeCount();
    set<int> randomPermutation;

    for (int i = 1; i <= num_nodes; i++)
    {
        randomPermutation.insert(i);
    }

    random_shuffle(randomPermutation.begin(), randomPermutation.end());

    for (int i : randomPermutation)
    {
        Node* queryNode = graph.getNode(i);

        set<Node*> V;
        set<Node*> L;

        vector<double> queryCoords = queryNode->getCoordinates(); // is this correct?? thelei &query coords    
        GreedySearch(graph, s, queryCoords, k, num_nodes, L, V);

        set<Node*> Nout;
        set_union(V.begin(), V.end(), L.begin(), L.end(), inserter(Nout, Nout.begin())); // using inserter to avoid overwriting

        RobustPrune(graph, queryNode, Nout, a, maxNodesEdges);

        for (Node* neighbor : Nout) 
        {
            // check degree of the node. If it exceeds R, apply RobustPrune again.
            if (graph.getNode(i)->getEdges().size() > maxNodesEdges) {
                //re-apply RobustPrune to ensure degree <= R
                RobustPrune(graph, queryNode, Nout, a, maxNodesEdges);
            } else {
                // update set since the degree constraint is not violated
                Nout.insert(graph.getNode(neighbor->getId()));           
            }
        }
    
    }

}