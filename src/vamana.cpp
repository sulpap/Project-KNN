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



// int Vamana(Graph &graph, vector<vector<double>> &coords, int R, double a, int int_L)
// {
//     generate_graph(graph, coords, R);

//     // s is the medoid of P and the start node
//     int medoidIndex = 8736; //findMedoid(coords);
//     cout << "Vamana writing, just found medoid!" << endl;
//     Node* s = graph.getNode(medoidIndex);

//     // make a random permutation of 1..n, to traverse the nodes in a random order
//     vector<int> randomPermutation(int_L);
//     iota(randomPermutation.begin(), randomPermutation.end(), 0); // fill with 0 to int_L
//     random_shuffle(randomPermutation.begin(), randomPermutation.end());

//     for (int i : randomPermutation)
//     {
//         Node* queryNode = graph.getNode(i);

//         if (!queryNode) {
//             cerr << "Invalid queryNode for index: " << i << endl;
//             continue; // Skip to the next iteration
//         }

//         set<Node*> V;
//         set<Node*> L;

//         vector<double> queryCoords = queryNode->getCoordinates();
//         GreedySearch(s, queryCoords, 1, int_L, L, V);
        
//         // initialize Nout with the current out-neighbors of queryNode
//         list<Node*> edges = queryNode->getEdges();
//         set<Node*> Nout(edges.begin(), edges.end());       

//         RobustPrune(graph, queryNode, Nout, a, R);
        
//         for (Node* neighbor : Nout) 
//         {
//             // check degree of the node. If it exceeds R, apply RobustPrune again.
//             if (static_cast<int>(graph.getNode(i)->getEdges().size()) > R) {
//                 //re-apply RobustPrune to ensure degree <= R
//                 RobustPrune(graph, queryNode, Nout, a, R);
//             } else {
//                 // update set since the degree constraint is not violated
//                 Nout.insert(graph.getNode(neighbor->getId()));           
//             }
//         }
    
//     }
//     return medoidIndex;
// }


int Vamana(Graph &graph, vector<vector<double>> &coords, int R, int a, int int_L) 
{
    generate_graph(graph, coords, R);

    int medoidIndex = 8736; //findMedoid(coords); // 8736
    cout << "Vamana writing, just found medoid!" << endl;
    Node* medoid = graph.getNode(medoidIndex);

    // make a random permutation of 1..n, to traverse the nodes in a random order
    vector<int> randomPermutation(int_L);
    iota(randomPermutation.begin(), randomPermutation.end(), 0); // fill with 0 to int_L
    random_shuffle(randomPermutation.begin(), randomPermutation.end());

    for (int id : randomPermutation) 
    { 
        cout << id << endl;

        set<Node*> V_set;
        set<Node*> L_set;
        
        GreedySearch(medoid, coords[id], 1, int_L, L_set, V_set);

        Node* sigma_i = graph.getNode(id);
        RobustPrune(graph, sigma_i, V_set, a, R);

        list<Node*> sigma_i_out = sigma_i->getEdges();

        for (auto node_j : sigma_i_out) 
        {
            list<Node*> j_out = node_j->getEdges();
            set<Node*> j_out_sigma_i(j_out.begin(), j_out.end());
            j_out_sigma_i.insert(sigma_i);

            if(static_cast<int>(j_out_sigma_i.size()) > R) 
            {
                RobustPrune(graph, node_j, j_out_sigma_i, a, R);
            } else {
                auto it = find(j_out.begin(), j_out.end(), sigma_i);

                if(it != j_out.end()) {         // sigma_i doesn't exist in j_out
                    node_j->addEdge(sigma_i);
                }
            }
        }
    }
    return medoidIndex;
}