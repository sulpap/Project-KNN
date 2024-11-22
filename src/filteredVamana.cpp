#include "../include/filteredVamana.hpp"
// #include "../include/filteredGreedySearch.hpp"
// #include "../include/filteredRobustPrune.hpp"
#include "../include/generate_graph.hpp"
#include "../include/FindMedoid.hpp"
#include <algorithm>
#include <iostream>
#include <random>
#include <chrono>
#include <unordered_map>

// database P is basically the graph

unordered_map<int, int> defineStartNodes(Graph &graph, const set<int> &F);
unordered_map<int, set<int>> computeLabelSets(Graph &graph);

void filteredVamana(Graph &graph, vector<vector<double>> &coords, int R, double a, int int_L, set<int> F, int taph)
{
    // 1. Initialize the graph to a sparse graph with neighbors of the same label
    generate_label_based_graph(graph, coords, F);

    // 2. Let s denote the medoid of P (graph)
    cout << "Start of medoid calculation..." << endl;

    auto start = chrono::high_resolution_clock::now();

    map<int, Node *> s = FindMedoid(graph, taph, F);

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "Medoid calculation took: " << duration.count() << " seconds." << endl;

    // 3. Let st(f) denote the start node for filter label f for every f ∈ F
    unordered_map<int, int> st_f = defineStartNodes(graph, F); // unordered map has the least complexity

// cout << "Start nodes for each filter label:" << endl;
// for (const auto& [label, node] : st_f) {
//     cout << "Label: " << label << ", Start Node: " << node << endl;
// }

    // 4. Let σ (randomPermutation) be a random permutation of [n]
    vector<int> randomPermutation(coords.size());
    iota(randomPermutation.begin(), randomPermutation.end(), 0);

    // Obtain a time-based seed and shuffle
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    shuffle(randomPermutation.begin(), randomPermutation.end(), default_random_engine(seed));

    // 5. Let Fx be the label-set for every x ∈ P
    unordered_map<int, set<int>> Fx = computeLabelSets(graph);

// cout << "Label sets for each node:" << endl;
// for (const auto& [nodeId, Fx] : Fx_all) {
//     cout << "Node " << nodeId << ": { ";
//     for (int label : Fx) {
//         cout << label << " ";
//     }
//     cout << "}" << endl;
// }

    for (int point_id : randomPermutation)
    {
        // 6. Let S_{F_{x_{σ(i)}}} = {st(f): f ∈ F_{x_{σ(i)}} }
    }
}

unordered_map<int, int> defineStartNodes(Graph &graph, const set<int> &F)
{
    unordered_map<int, int> start_nodes; // st_f / st(f)

    // traverse through the labels in F
    for (int f : F)
    {
        // find the first node in the graph with label f
        vector<int> nodesWithLabel = graph.findNodesWithLabel(f);
        if (!nodesWithLabel.empty())
        { // we assume that the first occurrence is the start node ???????????
            start_nodes[f] = nodesWithLabel[0];
        }
        else
        {
            cout << "No nodes found with label: " << f << endl;
        }
    }

    return start_nodes;
}

// for each node in the graph, iterate through its neighbors and collect the labels
unordered_map<int, set<int>> computeLabelSets(Graph &graph)
{
    unordered_map<int, set<int>> labelSets;

    for (const auto &[nodeId, nodePtr] : graph.getAdjList())
    {
        set<int> Fx;                    // label set for node x
        Fx.insert(nodePtr->getLabel()); // include the node's label itself

        for (Node *neighbor : nodePtr->getEdges())
        {
            Fx.insert(neighbor->getLabel()); // store labels of neighbors
        }

        labelSets[nodeId] = Fx; // store the label set for this node
    }

    return labelSets;
}

//     set<Node *> V_set;
//     set<Node *> L_set;
//     GreedySearch(medoid, coords[point_id], 1, int_L, L_set, V_set);

//     Node *sigma_i = graph.getNode(point_id);
//     RobustPrune(sigma_i, V_set, a, R);

//     list<Node *> sigma_i_out = sigma_i->getEdges();
//     for (auto node_j : sigma_i_out)
//     {
//         list<Node *> j_out = node_j->getEdges();

//         set<Node *> j_out_sigma_i(j_out.begin(), j_out.end());
//         j_out_sigma_i.insert(sigma_i);

//         if (static_cast<int>(j_out_sigma_i.size()) > R)
//         {
//             RobustPrune(node_j, j_out_sigma_i, a, R);
//         }
//         else
//         {
//             auto it = find(j_out.begin(), j_out.end(), sigma_i);
//             if (it == j_out.end())
//             { // sigma_i doesn't exist in j_out
//                 node_j->addEdge(sigma_i);
//             }
//         }
//     }