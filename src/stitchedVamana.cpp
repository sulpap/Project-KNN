#include "../include/stitchedVamana.hpp"
#include "../include/vamana.hpp"
#include "../include/filteredrobustprune.hpp"

// in stitchedVamana, we know the set of points beforehand
// stitchedVamana returns a stitched graph, that sonsists of a collection of subgraphs, one for each label.
// if there are no nodes for a label, then the graph would be empty.

using namespace std;

Graph stitchedVamana(vector<vector<double>> &coords, set<int> F, double a, int L_small, int R_small, int R_stitched, map<int, Node *>& medoids)
{
    // 1. Initialize G = (V, E) to an empty graph
    Graph G;

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
            // continue without calling vamana to avoid extra work
            continue;
        }

        // 4. Let Gf ← Vamana(Pf, α, R_small, L_small)
        Graph Gf;

        int medoidId = Vamana(Gf, Pf, R_small, a, L_small, f); // we also give f, because vamana creates the graph, so it 
                                                               // needs to add the values of the nodes, in which is the label
    
        // merge "stitch" graphs 
        G.graphUnion(move(Gf));

        Gf.clear(); // Gf's adj list is already cleared by graphUnion. clear the nodes here.

        // store the medoid node
        store_medoid(G, medoids, f, medoidId);
    }

    // foreach v ∈ V do
    for (auto& [nodeId, nodePtr] : G.getAdjList()) 
    {
        // 5. FilteredRobustPrune(v, N_out(v), α, R_stitched)
        list<Node *> temp = nodePtr->getEdges();
        set<Node*> N_out(temp.begin(), temp.end());
        FilteredRobustPrune(nodePtr, N_out, a, R_stitched);
    }

    return G;
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

void store_medoid(Graph &G, map<int, Node *>& medoids, int f, int medoidId)
{
    // find the medoid Node in Gf using its ID
    Node* medoidNode = G.getNode(medoidId);
    if (medoidNode != nullptr) 
    {
        // store the medoid: key is the label (f), value is the Node*
        medoids[f] = medoidNode;
    } 
    else 
    {
        cerr << "Error: Medoid ID " << medoidId << " not found in graph Gf for label " << f << endl;
    }
}