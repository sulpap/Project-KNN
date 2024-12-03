#include "../include/filteredVamana.hpp"
#include "../include/vamana.hpp"
#include "../include/filteredrobustprune.hpp"
#include "../include/generate_graph.hpp"

// in stitchedVamana, we know the set of points beforehand
// stitchedVamana does not return a stitched graph, but a collection of graphs, one for each label.
// if there are no nodes for a label, then the graph would be empty.

unordered_map<int, set<int>> compute_Fx(vector<vector<double>> &coords);
unordered_map<int, vector<vector<double>>> compute_PfMap(vector<vector<double>> &coords, set<int> F);

map<int, Graph> stitchedVamana(vector<vector<double>> &coords, set<int> F, double a, int L_small, int R_small, int R_stitched)
{
    // 1. Initialize G = (V, E) to an empty graph
    map<int, Graph> labelGraphs; // we initialize an empty map -> int = label, Graph = Gf, to store each Gf for label f

    // 2. Let Fx ⊆ F be the label-set for every x ∈ P
    unordered_map<int, set<int>> Fx = compute_Fx(coords); // Fx maps node IDs to their label sets 

    // 3. Let Pf ⊆ P be the set of points with label f ∈ F
    unordered_map<int, vector<vector<double>>> PfMap = compute_PfMap(coords, F); // Map each label f to its corresponding set of points (coordinates)

    // foreach f ∈ F do
    for (int f : F)
    {
        // get Pf (the nodes' coords) for this label
        vector<vector<double>> Pf = PfMap[f];

        // if Pf is empty, then we were given no nodes with this label
        if (Pf.empty())
        {
            Graph Gf;
            labelGraphs[f] = Gf; // add the empty graph without calling vamana
            continue;
        }

        // 4. Let Gf ← Vamana(Pf, α, R_small, L_small)
        Graph Gf;
        Vamana(Gf, Pf, R_small, a, L_small, f); // we also give f, because vamana creates the graph, so it 
                                                // needs to add the values of the nodes, in which is the label
        // store Gf                                        
        labelGraphs[f] = Gf; 
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

unordered_map<int, set<int>> compute_Fx(vector<vector<double>> &coords)
{
    unordered_map<int, set<int>> Fx;
    for (size_t i = 0; i < coords.size(); i++) 
    {
        if (coords[i].empty()) 
        {
            cerr << "Error: Empty coordinate vector at index " << i << std::endl;
            continue;
        }

        // first element is the label
        int label = static_cast<int>(coords[i][0]);

        // store i's label
        Fx[i].insert(label);
    }
    return Fx;
}

unordered_map<int, vector<vector<double>>> compute_PfMap(vector<vector<double>> &coords, set<int> F)
{
    unordered_map<int, vector<vector<double>>> PfMap;
    for (size_t i = 0; i < coords.size(); i++) 
    {
        if (coords[i].empty()) 
        {
            cerr << "Error: Empty coordinate vector at index " << i << std::endl;
            continue;
        }

        // first element is the label
        int label = static_cast<int>(coords[i][0]);

        // check if the label is in the filter set F
        if (F.find(label) != F.end()) 
        {
            // remaining elements are the coordinates
            vector<double> pointCoords(coords[i].begin() + 1, coords[i].end());

            // add the coordinates to the corresponding label in PfMap
            PfMap[label].push_back(pointCoords);
        }
    }
    return PfMap;
}