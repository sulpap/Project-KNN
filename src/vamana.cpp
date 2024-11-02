#include "../include/vamana.hpp"
#include "../include/greedysearch.hpp"
#include "../include/robustprune.hpp"
#include "../include/generate_graph.hpp"
#include "../include/utility.hpp"
#include <algorithm>
#include <iostream>
#include <random>

using namespace std;

// coords = coords of all vectors in P dataset (Graph)
// k = number of closest neighbors we need
// R = max edges
// randomPermutation = σ
// queryNode = σ(i), node with id

// L is the set of closest neighbors returned from Greedy
// V is the set of visited nodes returned from Greedy
// Nout are the possible candidates, out-neighbors.

// neighbor = j

int Vamana(Graph &graph, vector<vector<double>> &coords, int R, int a, int int_L)
{
    generate_graph(graph, coords, R);

    cout<<"initial graph:"<< endl;
    graph.printEdges();

    // s is the medoid of P and the start node
    int medoidIndex = findMedoid(coords);
    Node* s = graph.getNode(medoidIndex);

    // make a random permutation of 1..n, to traverse the nodes in a random order
    vector<int> randomPermutation(int_L);
    iota(randomPermutation.begin(), randomPermutation.end(), 0); // fill with 0 to int_L
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

        vector<double> queryCoords = queryNode->getCoordinates();
        GreedySearch(graph, s, queryCoords, 1, int_L, L, V);
        
        // initialize Nout with the current out-neighbors of queryNode
        list<Node*> edges = queryNode->getEdges();
        set<Node*> Nout(edges.begin(), edges.end());       

        RobustPrune(graph, queryNode, Nout, a, R);
        
        for (Node* neighbor : Nout) 
        {
            // check degree of the node. If it exceeds R, apply RobustPrune again.
            if (graph.getNode(i)->getEdges().size() > R) {
                //re-apply RobustPrune to ensure degree <= R
                RobustPrune(graph, queryNode, Nout, a, R);
            } else {
                // update set since the degree constraint is not violated
                Nout.insert(graph.getNode(neighbor->getId()));           
            }
        }
    
    }

    cout << "final graph:"<< endl;
    graph.printEdges();

    return medoidIndex;
}