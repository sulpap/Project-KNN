// #include "../include/filteredVamana.hpp"
// #include "../include/greedysearch.hpp"
// #include "../include/filteredrobustprune.hpp"
// #include "../include/FindMedoid.hpp"
// #include <algorithm>
// #include <iostream>
// #include <random>
// #include <chrono>
// #include <unordered_map>

// // database P is basically coords

// unordered_map<int, int> defineStartNodes(Graph &graph, const set<int> &F);
// unordered_map<int, set<int>> computeLabels(Graph &graph, set<int> F);

// // we declare label set f for each node, but the nodes have either one label, or none at all.

// Graph filteredVamana(vector<vector<double>> &coords, double a, int int_L, int R,  set<int> F, int taph)
// {
//     // 1. Initialize an empty graph
//     Graph graph;

//     // 2. Let s denote the medoid of P (graph)
//     cout << "Start of medoid calculation..." << endl;
//     auto start = chrono::high_resolution_clock::now();

//     map<int, Node *> s = FindMedoid(graph, taph, F);

//     auto end = chrono::high_resolution_clock::now();
//     chrono::duration<double> duration = end - start;
//     cout << "Medoid calculation took: " << duration.count() << " seconds." << endl;

//     // 3. Let st(f) denote the start node for filter label f for every f ∈ F
//     unordered_map<int, int> st_f = defineStartNodes(graph, F); // unordered map has the least complexity

// // cout << "Start nodes for each filter label:" << endl;
// // for (const auto& [label, node] : st_f) {
// //     cout << "Label: " << label << ", Start Node: " << node << endl;
// // }

//     // 4. Let σ (randomPermutation) be a random permutation of [n]
//     vector<int> randomPermutation(coords.size());
//     iota(randomPermutation.begin(), randomPermutation.end(), 0);

//     // Obtain a time-based seed and shuffle
//     unsigned seed = chrono::system_clock::now().time_since_epoch().count();
//     shuffle(randomPermutation.begin(), randomPermutation.end(), default_random_engine(seed));

//     // 5. Let Fx be the label-set for every x ∈ P
//     unordered_map<int, set<int>> Fx = computeLabels(graph, F); // set, but nodes have either one label or none at all (=all)

//     set<Node *> V; // initializing this set outside the loop, so that it accumulates nodes across iterations

//     // foreach i ∈ [n] do
//     for (int point_id : randomPermutation)
//     {
//         // 6. Let S_{F_{x_{σ(i)}}} = {st(f): f ∈ F_{x_{σ(i)}} }

//         // Get the label of the current node x_sigma(i)
//         set<int> Fx_sigma_i = Fx[point_id];

//         // Initialize S_{F_{x_{σ(i)}}} as the set of start nodes for the label
//         set<Node*> S_Fx_sigma_i;

//         for (int label : Fx_sigma_i) 
//         {
//             if (st_f.find(label) != st_f.end()) 
//             {
//                 Node* startNode = graph.getNode(st_f[label]);
//                 if (startNode != nullptr) 
//                 {   // Add the start node corresponding to this label to the set
//                     S_Fx_sigma_i.insert(startNode);
//                 }
//             } 
//             else 
//             {
//                 cout << "Warning: Start node for label " << label << " not found!" << endl;
//             }
//         }

//         // Also include nodes with label -1
//         for (const auto &[nodeId, nodePtr] : graph.getAdjList()) 
//         {
//             if (nodePtr->getLabel() == -1) 
//             {
//                 S_Fx_sigma_i.insert(nodePtr);
//             }
//         }

// // cout << "Node " << point_id << ": Start node for label " << Fx_sigma_i << ", Start node count: " << S_Fx_sigma_i.size() << endl;

//         // 7. Let [∅;V_{F_{x_{σ(i)}}}] ← FilteredGreedySearch(S_{F_{x_{σ(i)}}}, x_{σ(i)}, 0, L, F_{x_{σ(i)}})
//         set<Node *> V_Fx_sigma_i;
//         set<Node *> L_set;
//         vector<double> queryCoords = coords[point_id];

//         FilteredGreedySearch(S_Fx_sigma_i, queryCoords, 0, int_L, L_set, V_Fx_sigma_i, Fx_sigma_i);

// // cout << "After FGS" << endl;

//         // 8. V ← V ∪ V_{F_{x_{σ(i)}}}
//         V.insert(V_Fx_sigma_i.begin(), V_Fx_sigma_i.end());

//         // 9. Run FilteredRobustPrune(σ(i), V_{F_{x_{σ(i)}}}, α, R) to update out-neighbors of σ(i)
//         Node *sigma_i = graph.getNode(point_id);
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
//             // 10. Update N_out(j) ← N_out(j) ∪ {σ(i)}
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
//                 // 11. Run FilteredRobustPrune(j, N_out(j), α, R) to update out-neighbors of j
//                 set<Node *> j_out_set(j_out.begin(), j_out.end());
//                 FilteredRobustPrune(node_j, j_out_set, a, R);
                
//             }
//         }
//     }
//     return graph;
// }

// unordered_map<int, int> defineStartNodes(Graph &graph, const set<int> &F)
// {
//     unordered_map<int, int> start_nodes; // st(f) for every f ∈ F

//     // traverse through the labels in F
//     for (int f : F)
//     {
//         // find the first node in the graph with label f
//         vector<int> nodesWithLabel = graph.findNodesWithLabel(f);

//         // also find the nodes without a label
//         vector<int> nodesWithoutLabel = graph.findNodesWithLabel(-1);
//         // and store them in the same buffer
//         nodesWithLabel.insert(nodesWithLabel.end(), nodesWithoutLabel.begin(), nodesWithoutLabel.end());

//         if (!nodesWithLabel.empty())
//         { // we assume that the first occurrence is the start node ------------------------------------------------------
//             start_nodes[f] = nodesWithLabel[0];
//         }
//         else
//         {
//             cout << "No nodes found with label: " << f << endl;
//         }
//     }

//     return start_nodes;
// }

// // find each node's label
// unordered_map<int, set<int>> computeLabels(Graph &graph, set<int> F) 
// {
//     unordered_map<int, set<int>> labels;

//     // iterate through all nodes in the graph
//     for (const auto &[nodeId, nodePtr] : graph.getAdjList()) 
//     {
//         // get the label from this node
//         int label = nodePtr->getLabel();

//         // if it has no label, we treat it like it has them all
//         if (label == -1) 
//         {
//             labels[nodeId] = F; // assign all labels from the label set F
//         } else 
//         { // if it has a label, we store it
//             labels[nodeId] = {label};
//         }
//     }

//     return labels;
// }

// // for each node in the graph, iterate through its neighbors and collect the labels --> will possibly need if the labels are a set = many labels
// // unordered_map<int, set<int>> computeLabelSets(Graph &graph)
// // {
// //     unordered_map<int, set<int>> labelSets;
// //     for (const auto &[nodeId, nodePtr] : graph.getAdjList())
// //     {
// //         set<int> Fx;                    // label set for node x
// //         Fx.insert(nodePtr->getLabel()); // include the node's label itself
// //         for (Node *neighbor : nodePtr->getEdges())
// //         {
// //             Fx.insert(neighbor->getLabel()); // store labels of neighbors
// //         }
// //         labelSets[nodeId] = Fx; // store the label set for this node
// //     }
// //     return labelSets;
// // }




// // old:
// //     set<Node *> V_set;
// //     set<Node *> L_set;
// //     GreedySearch(medoid, coords[point_id], 1, int_L, L_set, V_set);

// //     Node *sigma_i = graph.getNode(point_id);
// //     RobustPrune(sigma_i, V_set, a, R);

// //     list<Node *> sigma_i_out = sigma_i->getEdges();
// //     for (auto node_j : sigma_i_out)
// //     {
// //         list<Node *> j_out = node_j->getEdges();

// //         set<Node *> j_out_sigma_i(j_out.begin(), j_out.end());
// //         j_out_sigma_i.insert(sigma_i);

// //         if (static_cast<int>(j_out_sigma_i.size()) > R)
// //         {
// //             RobustPrune(node_j, j_out_sigma_i, a, R);
// //         }
// //         else
// //         {
// //             auto it = find(j_out.begin(), j_out.end(), sigma_i);
// //             if (it == j_out.end())
// //             { // sigma_i doesn't exist in j_out
// //                 node_j->addEdge(sigma_i);
// //             }
// //         }
// //     }