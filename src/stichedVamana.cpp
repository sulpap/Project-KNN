#include "../include/filteredVamana.hpp"
#include "../include/vamana.hpp"
#include "../include/filteredrobustprune.hpp"
#include "../include/generate_graph.hpp"
#include <unordered_map>

unordered_map<int, set<int>> computeLabels(Graph &graph);
vector<vector<double>> computePoints(set<int> F, unordered_map<int, set<int>> Fx, vector<vector<double>> &coords);

Graph stichedVamana(vector<vector<double>> &coords, set<int> F, double a, int L_small, int R_small, int R_stiched)
{
    // 1. Initialize G = (V, E) to an empty graph
    Graph G;
    // generate_label_based_graph(G, coords, F);

    // 2. Let Fx ⊆ F be the label-set for every x ∈ P
    unordered_map<int, set<int>> Fx = computeLabels(G); 

    // 3. Let Pf ⊆ P be the set of points with label f ∈ F
    vector<vector<double>> Pf = computePoints(F, Fx, coords);

    // foreach f ∈ F do
    for (int f : F)
    {
        // 4. Let Gf ← Vamana(Pf, α, R_small, L_small)
        Graph Gf;
        Vamana(Gf, Pf, R_small, a, L_small);

        // merge ("stich") Gf into G
        G.graphUnion(Gf);
    }

    // foreach v ∈ V do
    for (auto &[nodeId, nodePtr] : G.getAdjList())
    {
        // 5. FilteredRobustPrune(v, N_out(v), α, R_stiched)
        set<Node *> N_out(nodePtr->getEdges().begin(), nodePtr->getEdges().end());
        FilteredRobustPrune(nodePtr, N_out, a, R_stiched);
    }

    return G;
}

unordered_map<int, set<int>> computeLabels(Graph &graph) 
{
    unordered_map<int, set<int>> labels;

    // iterate through all nodes in the graph
    for (const auto &[nodeId, nodePtr] : graph.getAdjList()) 
    {
        // get and store the label for this node
        labels[nodeId] = {nodePtr->getLabel()};
    }

    return labels;
}

vector<vector<double>> computePoints(set<int> F, unordered_map<int, set<int>> Fx, vector<vector<double>> &coords)
{
    vector<vector<double>> Pf; // to store the coordinates of points with label f
    for (int f : F)
    {
        for (const auto &[nodeId, labels] : Fx)
        {
            if (labels.find(f) != labels.end()) // if node has label f
            {
                Pf.push_back(coords[nodeId]); // add its coordinates to Pf
            }
        }
    }
    return Pf;
}