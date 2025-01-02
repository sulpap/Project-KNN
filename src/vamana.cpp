#include "../include/vamana.hpp"
#include "../include/greedysearch.hpp"
#include "../include/robustprune.hpp"
#include "../include/generate_graph.hpp"
#include "../include/utility.hpp"
#include <algorithm>
#include <iostream>
#include <random>
#include <chrono>

// coords = coords of all vectors in P dataset (Graph)
// randomPermutation = σ

// L is the set of closest neighbors returned from Greedy
// V is the set of visited nodes returned from Greedy
// sigma_i_out are the possible candidates, out-neighbors.

// calls findMedoid and prints time taken
// int calculate_medoid(vector<vector<double>> &coords)
// {
//     cout << "Start of medoid calculation..." << endl;

//     auto start = chrono::high_resolution_clock::now();

//     int medoidIndex = findMedoid(coords);

//     auto end = chrono::high_resolution_clock::now();
//     chrono::duration<double> duration = end - start;

//     cout << "Medoid calculation took: " << duration.count() << " seconds." << endl;

//     return medoidIndex;
// }

int Vamana(Graph &graph, vector<Node *> &coords, int R, double a, int int_L)
{
    // unordered_map<int, int> indexes; // <position, id>

    vector<vector<double>> actual_coords;

    for (Node *node : coords) {
        actual_coords.push_back(node->getCoordinates());
    }

    generate_graph(graph, coords, R);

    int medoidNodeId = findMedoid(actual_coords);

    Node *medoid = graph.getNode(coords[medoidNodeId]->getId());

    if (medoid == nullptr)
    {
        cerr << "Error: Medoid node with ID " << medoidNodeId << " not found in the graph." << endl;
        return -1;
    }

    // make a random permutation of 1..n, to traverse the nodes in a random order
    vector<int> randomPermutation(coords.size());                // Size of vector randomPermutation = number of points in dataset = number of vectors in coords
    iota(randomPermutation.begin(), randomPermutation.end(), 0); // fills with numbers from 0 to coords.size() - 1

    // obtain a time-based seed:
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    shuffle(randomPermutation.begin(), randomPermutation.end(), default_random_engine(seed));

    for (int point_id : randomPermutation)
    {
        set<Node *> V_set;
        set<Node *> L_set;
        GreedySearch(medoid, actual_coords[point_id], 1, int_L, L_set, V_set);

        Node *sigma_i = graph.getNode(coords[point_id]->getId());
        if (sigma_i == nullptr)
        {
            cerr << "Error: Node with ID " << coords[point_id]->getId() << " not found in the graph." << endl;
            continue; // skip this iteration or handle the error appropriately
        }

        RobustPrune(sigma_i, V_set, a, R);

        list<Node *> sigma_i_out = sigma_i->getEdges();
        for (auto node_j : sigma_i_out)
        {
            list<Node *> j_out = node_j->getEdges();

            set<Node *> j_out_sigma_i(j_out.begin(), j_out.end());
            j_out_sigma_i.insert(sigma_i);

            if (static_cast<int>(j_out_sigma_i.size()) > R)
            {
                RobustPrune(node_j, j_out_sigma_i, a, R);
            }
            else
            {
                auto it = find(j_out.begin(), j_out.end(), sigma_i);
                if (it == j_out.end())
                { // sigma_i doesn't exist in j_out
                    node_j->addEdge(sigma_i);
                }
            }
        }
    }

    return coords[medoidNodeId]->getId();
}