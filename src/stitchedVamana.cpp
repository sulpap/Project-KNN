#include "../include/filteredVamana.hpp"
#include "../include/vamana.hpp"
#include "../include/filteredrobustprune.hpp"
#include "../include/generate_graph.hpp"

// in stitchedVamana, we know the set of points (nodes) beforehand, so we already have their labels.
// stitchedVamana does not return a stitched graph, but a collection of graphs, one for each label

map<int, set<int>> compute_Fx(const vector<Node*>& nodes);
map<int, vector<vector<double>>> compute_PfMap(const vector<Node*>& nodes, const set<int>& F); 

map<int, Graph> stitchedVamana(vector<Node*> &nodes, set<int> F, double a, int L_small, int R_small, int R_stitched)
{
    // 1. Initialize G = (V, E) to an empty graph
    map<int, Graph> labelGraphs; // we initialize an empty map -> int = label, Graph = Gf, to store each Gf for label f

    // 2. Let Fx ⊆ F be the label-set for every x ∈ P
    map<int, set<int>> Fx = compute_Fx(nodes); // Fx maps node IDs to their label sets

    // 3. Let Pf ⊆ P be the set of points with label f ∈ F
    map<int, vector<vector<double>>> PfMap = compute_PfMap(nodes, F); // Map each label f to its corresponding set of points (coordinates)

    // foreach f ∈ F do
    for (int f : F)
    {
        // get Pf (the nodes' coords) for this label
        vector<vector<double>> Pf = PfMap[f];

        // if Pf is empty, then we were given no nodes with this label
        if (Pf.empty())
        {
            cout << "There are no nodes with label " << f << "." << endl;
            continue;
        }

        // 4. Let Gf ← Vamana(Pf, α, R_small, L_small)
        Graph Gf;
        Vamana(Gf, Pf, R_small, a, L_small, f); // we also give f, because vamana creates the graph, so it 
                                                // needs to add the values of the nodes, in which is the label
                                                
        // merge ("stitch") Gf into G
        labelGraphs[f] = Gf; // store Gf
    }

    for (auto& [label, Gf] : labelGraphs) 
    {
        // foreach v ∈ V do
        for (auto& [nodeId, nodePtr] : Gf.getAdjList()) 
        {
            // 5. FilteredRobustPrune(v, N_out(v), α, R_stitched)
            list<Node *> temp = nodePtr->getEdges();
            set<Node*> N_out(temp.begin(), temp.end());
            FilteredRobustPrune(nodePtr, N_out, a, R_stitched);
        }
    }

    return labelGraphs;
}

map<int, set<int>> compute_Fx(const vector<Node*>& nodes) 
{
    map<int, set<int>> Fx;
    for (Node* node : nodes) 
    {
        // add the node's label to its label set
        Fx[node->getId()].insert(node->getLabel());
    }
    return Fx;
}

map<int, vector<vector<double>>> compute_PfMap(const vector<Node*>& nodes, const set<int>& F) 
{
    map<int, vector<vector<double>>> PfMap;
    for (Node* node : nodes) 
    {
        int label = node->getLabel();
        if (F.find(label) != F.end()) 
        {
            PfMap[label].push_back(node->getCoordinates());
        }
    }
    return PfMap;
}