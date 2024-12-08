#include "../include/filteredVamana.hpp"
#include "../include/filteredGreedySearch.hpp"
#include "../include/filteredrobustprune.hpp"
#include "../include/FindMedoid.hpp"
#include "../include/generate_graph.hpp"
#include <algorithm>
#include <iostream>
#include <random>
#include <chrono>
#include <cassert>

Graph filteredVamana(vector<vector<double>> &coords, double a, int int_L, int R,  set<int> F, int taph, map<int, Node *>& st_f) {
    Graph G;
    initialize_graph(G, coords);
    st_f = FindMedoid(G, taph, F);

    vector<int> randomPermutation(coords.size());
    iota(randomPermutation.begin(), randomPermutation.end(), 0);        // ξεκινάει από το 0 μέχρι και coords.size - 1    
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();     // obtain a time-based seed and shuffle
    shuffle(randomPermutation.begin(), randomPermutation.end(), default_random_engine(seed));

    for (int point_id : randomPermutation) {        // Τα ids στην initialize_graph πάνε και αυτά από 0 μέχρι και coords.size - 1
        Node* point = G.getNode(point_id);
        int label = point->getLabel();

        Node* start_node = st_f[label];
        set<Node*> L_set;
        set<Node*> V_set;
        set<Node*> S_set;
        S_set.insert(start_node);
        vector<double> coordinates = point->getCoordinates();
        int k = 0;
        set<int> F;
        F.insert(label);
        FilteredGreedySearch(S_set, coordinates, k, int_L, L_set, V_set, F);
        assert(L_set.size() == 0);

        FilteredRobustPrune(point, V_set, a, R);

        list<Node*> point_out = point->getEdges();
        for(auto j : point_out) {
            j->addEdge(point);      // addEdge won't add point as neighbor of j if that's already the case
            list<Node*> j_out = j->getEdges();

            if(static_cast<int>(j_out.size()) > R) {
                set<Node*> V_set(j_out.begin(), j_out.end());       // We initialize V with out neighbors of j
                FilteredRobustPrune(j, V_set, a, R);
            }
        }
    }
    return G;
}

// // database P is basically coords
// // we declare label set f for each node, even though the nodes have only one label, so the set always contains of one element.

// Graph filteredVamana(vector<vector<double>> &coords, double a, int int_L, int R, set<int> F, int taph, map<int, Node *> &st_f)
// {
//     // 1. Initialize an empty graph
//     Graph G;

//     // fill the graph with nodes based on the coords info (label & point coords)
//     initialize_graph(G, coords);

//     // 2. Let st(f) denote the start node for filter label f for every f ∈ F
//     st_f = FindMedoid(G, taph, F);

//     // 3. Let σ (randomPermutation) be a random permutation of [n]
//     vector<int> randomPermutation(coords.size());
//     iota(randomPermutation.begin(), randomPermutation.end(), 0);

//     // obtain a time-based seed and shuffle
//     unsigned seed = chrono::system_clock::now().time_since_epoch().count();
//     shuffle(randomPermutation.begin(), randomPermutation.end(), default_random_engine(seed));

//     // 4. Let Fx be the label-set for every x ∈ P
//     unordered_map<int, set<int>> Fx = compute_Fx(G, F);

//     // set<Node *> V; // initializing this set outside the loop, so that it accumulates nodes across iterations

//     // foreach i ∈ [n] do
//     for (int point_id : randomPermutation)
//     {
//         // 5. Let S_{F_{x_{σ(i)}}} = {st(f): f ∈ F_{x_{σ(i)}} }

//         // get the label of the current node x_sigma(i)
//         set<int> Fx_sigma_i = Fx[point_id];

//         // initialize S_{F_{x_{σ(i)}}} as the set of start nodes for the label
//         set<Node *> S_Fx_sigma_i;

//         for (int label : Fx_sigma_i)
//         {
//             // see if there exists a start node for this label
//             auto it = st_f.find(label);
//             if (it != st_f.end())
//             {
//                 Node *startNode = it->second; // can use st_f[label]; to access the node, but we use the iterator to avoid a second lookup (first was for find)
//                 if (startNode != nullptr)
//                 { // add the start node corresponding to this label to the set
//                     S_Fx_sigma_i.insert(startNode);
//                 }
//             }
//             else
//             {
//                 cout << "Warning: Start node for label " << label << " not found!" << endl;
//             }
//         }

//         // 6. Let [∅;V_{F_{x_{σ(i)}}}] ← FilteredGreedySearch(S_{F_{x_{σ(i)}}}, x_{σ(i)}, 0, L, F_{x_{σ(i)}})
//         set<Node *> V_Fx_sigma_i;
//         set<Node *> L_set;
//         vector<double> queryCoords = (G.getNode(point_id))->getCoordinates();

//         FilteredGreedySearch(S_Fx_sigma_i, queryCoords, 0, int_L, L_set, V_Fx_sigma_i, Fx_sigma_i);

//         // 7. V ← V ∪ V_{F_{x_{σ(i)}}} -------- unnecessary step
//         // V.insert(V_Fx_sigma_i.begin(), V_Fx_sigma_i.end());

//         // 8. Run FilteredRobustPrune(σ(i), V_{F_{x_{σ(i)}}}, α, R) to update out-neighbors of σ(i)
//         Node *sigma_i = G.getNode(point_id);
//         if (!sigma_i)
//         {
//             cout << "Warning: Node " << point_id << " is null, skipping..." << endl;
//             continue;
//         }
//         FilteredRobustPrune(sigma_i, V_Fx_sigma_i, a, R);

//         // foreach j ∈ N_out(σ(i)) do
//         list<Node *> sigma_i_out = sigma_i->getEdges();
//         for (auto node_j : sigma_i_out)
//         {
//             // 9. Update N_out(j) ← N_out(j) ∪ {σ(i)}
//             list<Node *> j_out = node_j->getEdges();
//             auto it = find(j_out.begin(), j_out.end(), sigma_i);
//             if (it == j_out.end())
//             { // sigma_i doesn't exist in j_out
//                 node_j->addEdge(sigma_i);
//             }

//             // if |N_out(j)| > R then
//             j_out = node_j->getEdges();
//             if (static_cast<int>(j_out.size()) > R)
//             {
//                 // 10. Run FilteredRobustPrune(j, N_out(j), α, R) to update out-neighbors of j
//                 set<Node *> j_out_set(j_out.begin(), j_out.end());
//                 FilteredRobustPrune(node_j, j_out_set, a, R);
//             }
//         }
//     }
//     return G;
// }

void initialize_graph(Graph &G, const vector<vector<double>> &coords)
{
    // initialize nodes from coords
    for (size_t i = 0; i < coords.size(); i++)
    {
        if (coords[i].empty())
        {
            cerr << "Error: Empty coordinate vector at index " << i << endl;
            continue;
        }

        // first element is the label
        int label = static_cast<int>(coords[i][0]);

        // remaining elements are the coordinates
        vector<double> pointCoords(coords[i].begin() + 1, coords[i].end());

        // create a new Node
        Node *newNode = new Node(static_cast<int>(i), pointCoords, {}, label); // id: i, coords, no edges, label

        // add the node to the graph
        G.addNode(newNode);
    }
}

unordered_map<int, set<int>> compute_Fx(Graph &G, set<int> F)
{
    // find each node's label and store it in Fx
    unordered_map<int, set<int>> Fx;

    // iterate through all nodes in the graph
    for (const auto &[nodeId, nodePtr] : G.getAdjList())
    {
        // get the label from this node
        int label = nodePtr->getLabel();

        Fx[nodeId] = {label};
    }

    return Fx;
}