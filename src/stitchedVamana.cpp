#include "../include/filteredVamana.hpp"
#include "../include/vamana.hpp"
#include "../include/filteredrobustprune.hpp"
#include "../include/generate_graph.hpp"

map<int, set<int>> computeLabels(vector<vector<double>> &coords, vector<int> &F);
map<int, vector<vector<double>>> computePoints(const vector<vector<double>> &coords, const vector<int> &F);

// in stitchedVamana, we know the set of points beforehand.
// so label set F has labels whose position corresponds to the label of the point at the same position in P.
// this is why F must be a vector<int> of the same size as coords.

Graph stitchedVamana(vector<vector<double>> &coords, vector<int> F, double a, int L_small, int R_small, int R_stitched)
{
    // 1. Initialize G = (V, E) to an empty graph
    Graph G;

    // 2. Let Fx ⊆ F be the label-set for every x ∈ P
    map<int, set<int>> Fx = computeLabels(coords, F); 

    // 3. Let Pf ⊆ P be the set of points with label f ∈ F
    map<int, vector<vector<double>>> groupedPoints = computePoints(coords, F);

    // this is to keep track of processed labels to avoid extra work
    set<int> processedLabels;

    // foreach f ∈ F do
    for (int f : F)
    {
        if (processedLabels.find(f) != processedLabels.end()) 
        {
            // skip if this label was already processed
            continue;
        }
        processedLabels.insert(f);

        // get Pf
        vector<vector<double>> Pf = groupedPoints[f]; // how do I get Pf?

        // 4. Let Gf ← Vamana(Pf, α, R_small, L_small)
        Graph Gf;
        Vamana(Gf, Pf, R_small, a, L_small, f); // we also give f, because vamana creates the graph, so it 
                                                // needs to add the values of the nodes, in which is the label
        // merge ("stitch") Gf into G
        G.graphUnion(Gf);
    }

    // foreach v ∈ V do
    for (auto &[nodeId, nodePtr] : G.getAdjList())
    {
        // 5. FilteredRobustPrune(v, N_out(v), α, R_stitched)
        set<Node *> N_out(nodePtr->getEdges().begin(), nodePtr->getEdges().end());
        FilteredRobustPrune(nodePtr, N_out, a, R_stitched);
    }

    return G;
}

// <int, set<int>> is a point id and its label
map<int, set<int>> computeLabels(vector<vector<double>> &coords, vector<int> &F)
{
    map<int, set<int>> labels;

    // make sure F has a label for each point
    if (coords.size() != F.size()) {
        cerr << "Error: The size of coords does not match the size of labels in F!" << endl;
        return labels;
    }

    // for each point in coords
    for (int i = 0; i < coords.size(); ++i) {
        // store its label from F
        labels[i].insert(F[i]);
    }

    return labels;
}
// map<int, set<int>> computeLabels(Graph &graph) 
// {
//     map<int, set<int>> labels;

//     // iterate through all nodes in the graph
//     for (const auto &[nodeId, nodePtr] : graph.getAdjList()) 
//     {
//         // get and store the label for this node
//         labels[nodeId] = {nodePtr->getLabel()};
//         printf("label of %d is: %d\n", nodeId, labels[nodeId]);
//     }

//     return labels;
// }


map<int, vector<vector<double>>> computePoints(const vector<vector<double>> &coords, const vector<int> &F)
{
    map<int, vector<vector<double>>> labelToPoints;

    // Ensure F has the same size as coords
    if (coords.size() != F.size()) 
    {
        cerr << "Error: The size of coords does not match the size of labels in F!" << endl;
        return labelToPoints;
    }

    // Group points in coords by their label in F
    for (int i = 0; i < coords.size(); ++i) 
    {
        int label = F[i];
        labelToPoints[label].push_back(coords[i]);
    }

    return labelToPoints;
}
// vector<vector<double>> computePoints(set<int> F, map<int, set<int>> Fx, vector<vector<double>> &coords)
// {
//     vector<vector<double>> Pf; // to store the coordinates of points with label f
//     for (int f : F) 
//     {
//         for (const auto &[nodeId, labels] : Fx)
//         {
//             if (labels.find(f) != labels.end()) // if node has label f
//             {
//                 Pf.push_back(coords[nodeId]); // add its coordinates to Pf
//             }
//         }
//     }
//     return Pf;
// }
