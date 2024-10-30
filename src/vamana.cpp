#include "../include/vamana.hpp"
#include "../include/greedysearch.hpp"
#include "../include/robustprune.hpp"
#include "../include/generate_graph.hpp"
#include "../include/utility.hpp"
#include <algorithm>
#include <iostream>
#include <random>

using namespace std;

// coords = coords of all vectors in P dataset
// k = number of closest neighbors we need
// maxNodesEdges = R
// randomPermutation = σ
// queryNode = σ(i), node with id

// L is the set of closest neighbors returned from Greedy
// V is the set of visited nodes returned from Greedy
// Nout are the possible candidates, out-neighbors.

// neighbor = j

void Vamana(Graph &graph, vector<vector<double>> &coords, int maxNodesEdges, int k, int a)
{
    generate_graph(graph, coords, maxNodesEdges);

    cout<<"initial graph:"<< endl;
    graph.printEdges();

    // s is the medoid of P and the start node
    Node* s = graph.getNode(findMedoid(coords));

    // make a random permutation of 1..n, to traverse the nodes in a random order
    int num_nodes = graph.getNodeCount();
    vector<int> randomPermutation(num_nodes);

    iota(randomPermutation.begin(), randomPermutation.end(), 0); // fill with 0 to num_nodes

    random_shuffle(randomPermutation.begin(), randomPermutation.end());

    for (int i : randomPermutation)
    {
        Node* queryNode = graph.getNode(i);

        if (!queryNode) {
            cerr << "Invalid queryNode for index: " << i << endl;
            continue; // Skip to the next iteration
        }

        set<Node*> V;
        set<Node*> L;

        vector<double> queryCoords = queryNode->getCoordinates(); // is this correct?? thelei &query coords   
        GreedySearch(graph, s, queryCoords, k, num_nodes, L, V);
        
        // initialize Nout with the current out-neighbors of queryNode
        list<Node*> edges = queryNode->getEdges();
        set<Node*> Nout(edges.begin(), edges.end());       

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
    cout << "final graph:"<< endl;
    graph.printEdges();
}

// * where do we deleet nodes and do we need to do that 