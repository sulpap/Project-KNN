/* FilteredVamana implementation file:
   Contains the serial implementation of filteredVamana, which consists of two implementations:
   1) a simpler and faster version
   2) a version following striclty the pseudocode, which gives 
    the same results, but is slower.

   To switch between the versions, just comment/uncomment.
*/

#include "../include/filteredVamana.hpp"
#include "../include/filteredGreedySearch.hpp"
#include "../include/filteredrobustprune.hpp"
#include "../include/FindMedoid.hpp"
#include "../include/generate_graph.hpp"
#include "../include/utility.hpp"               // due to use of euclidean_distance_of_nodes
#include <algorithm>
#include <iostream>
#include <random>
#include <chrono>
#include <cassert>
#include <pthread.h>

Graph filteredVamana(vector<vector<double>> &coords, double a, int int_L, int R, set<int> F, int taph, map<int, Node *> &st_f)
{
    // 1. Initialize an empty graph
    Graph G;

    // fill the graph with nodes based on the coords info (label & point coords)
    initialize_graph(G, coords);
    // generate_label_based_graph(G, coords);

    // 2. Let st(f) denote the start node for filter label f for every f ∈ F
    st_f = FindMedoid(G, taph, F);

    // 3. Let σ (randomPermutation) be a random permutation of [n]
    vector<int> randomPermutation(coords.size());
    iota(randomPermutation.begin(), randomPermutation.end(), 0);            // starts from 0 until coords.size - 1
    unsigned seed = chrono::system_clock::now().time_since_epoch().count(); // obtain a time-based seed and shuffle
    shuffle(randomPermutation.begin(), randomPermutation.end(), default_random_engine(seed));

    // foreach i ∈ [n] do
    for (int point_id : randomPermutation) // ids in initialize_graph are also from 0 to coords.size - 1
    {
        Node *point = G.getNode(point_id);
        int label = point->getLabel();

        Node *start_node = st_f[label];
        set<Node *> L_set;
        set<Node *> V_set;
        set<Node *> S_set;

        S_set.insert(start_node);
        
        vector<double> coordinates = point->getCoordinates();
        int k = 0;

        set<int> F;
        F.insert(label);

        FilteredGreedySearch(S_set, coordinates, k, int_L, L_set, V_set, F);
        assert(L_set.size() == 0);

        FilteredRobustPrune(point, V_set, a, R);

        list<Node *> point_out = point->getEdges();
        for (auto j : point_out)
        {
            j->addEdge(point); // addEdge won't add point as neighbor of j if that's already the case
            list<Node *> j_out = j->getEdges();

            if (static_cast<int>(j_out.size()) > R)
            {
                set<Node *> V_set(j_out.begin(), j_out.end()); // We initialize V with out neighbors of j
                FilteredRobustPrune(j, V_set, a, R);
            }
        }
    }
    return G;
}

// (serial) version based strictly on the pseudocode:

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
//     unordered_map<int, set<int>> Fx = computeFx(G);

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