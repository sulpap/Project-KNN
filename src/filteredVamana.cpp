#include "../include/filteredVamana.hpp"
#include "../include/filteredGreedySearch.hpp"
#include "../include/filteredrobustprune.hpp"
#include "../include/FindMedoid.hpp"
#include "../include/generate_graph.hpp"
#include <algorithm>
#include <iostream>
#include <random>
#include <chrono>
#include <unordered_map>

// database P is basically coords
// we declare label set f for each node, even though the nodes have only one label, so the set always contains of one element.

void initialize_graph(Graph &G, const vector<vector<double>> &coords);
unordered_map<int, int> compute_st_f(Graph &graph, const set<int> &F);
unordered_map<int, set<int>> compute_Fx(Graph &graph, set<int> F);

Graph filteredVamana(vector<vector<double>> &coords, double a, int int_L, int R,  set<int> F, int taph)
{
    // 1. Initialize an empty graph
    Graph G;

    // fill the graph with nodes based on the coords info (label & point coords)
    initialize_graph(G, coords);
    
    // 2. Let s denote the medoid of P (graph)
    map<int, Node *> s = FindMedoid(G, taph, F);

    // 3. Let st(f) denote the start node for filter label f for every f ∈ F
    unordered_map<int, int> st_f = compute_st_f(G, F); // unordered map has the least complexity

    // 4. Let σ (randomPermutation) be a random permutation of [n]
    vector<int> randomPermutation(coords.size());
    iota(randomPermutation.begin(), randomPermutation.end(), 0);

    // obtain a time-based seed and shuffle
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    shuffle(randomPermutation.begin(), randomPermutation.end(), default_random_engine(seed));

    // 5. Let Fx be the label-set for every x ∈ P
    unordered_map<int, set<int>> Fx = compute_Fx(G, F);

    set<Node *> V; // initializing this set outside the loop, so that it accumulates nodes across iterations

    // this is to keep track of already processed labels, to avoid extra work
    // set<int> processedLabels;

    // foreach i ∈ [n] do
    for (int point_id : randomPermutation)
    {
        // 6. Let S_{F_{x_{σ(i)}}} = {st(f): f ∈ F_{x_{σ(i)}} }

        // get the label of the current node x_sigma(i)
        set<int> Fx_sigma_i = Fx[point_id];

        // initialize S_{F_{x_{σ(i)}}} as the set of start nodes for the label
        set<Node*> S_Fx_sigma_i;

        for (int label : Fx_sigma_i) 
        {
            if (st_f.find(label) != st_f.end()) 
            {
                Node* startNode = G.getNode(st_f[label]);
                if (startNode != nullptr) 
                {   // add the start node corresponding to this label to the set
                    S_Fx_sigma_i.insert(startNode);
                }
            } 
            else 
            {
                cout << "Warning: Start node for label " << label << " not found!" << endl;
            }
        }

        // 7. Let [∅;V_{F_{x_{σ(i)}}}] ← FilteredGreedySearch(S_{F_{x_{σ(i)}}}, x_{σ(i)}, 0, L, F_{x_{σ(i)}})
        set<Node *> V_Fx_sigma_i;
        set<Node *> L_set;
        vector<double> queryCoords = (G.getNode(point_id))->getCoordinates();

        FilteredGreedySearch(S_Fx_sigma_i, queryCoords, 0, int_L, L_set, V_Fx_sigma_i, Fx_sigma_i);

// cout << "After FGS" << endl;

        // 8. V ← V ∪ V_{F_{x_{σ(i)}}}
        V.insert(V_Fx_sigma_i.begin(), V_Fx_sigma_i.end());

        // 9. Run FilteredRobustPrune(σ(i), V_{F_{x_{σ(i)}}}, α, R) to update out-neighbors of σ(i)
        Node *sigma_i = G.getNode(point_id);
        if (!sigma_i) 
        {
            cout << "Warning: Node " << point_id << " is null, skipping..." << endl;
            continue;
        }
        FilteredRobustPrune(sigma_i, V_Fx_sigma_i, a, R);

// cout << "After FRP" << endl;

        // foreach j ∈ N_out(σ(i)) do
        list<Node *> sigma_i_out = sigma_i->getEdges();
        for (auto node_j : sigma_i_out)
        {
            // 10. Update N_out(j) ← N_out(j) ∪ {σ(i)}
            list<Node *> j_out = node_j->getEdges();
            auto it = find(j_out.begin(), j_out.end(), sigma_i);
            if (it == j_out.end())
            { // sigma_i doesn't exist in j_out
                node_j->addEdge(sigma_i);
            }

            // if |N_out(j)| > R then
            j_out = node_j->getEdges();
            if (static_cast<int>(j_out.size()) > R)
            {
                // 11. Run FilteredRobustPrune(j, N_out(j), α, R) to update out-neighbors of j
                set<Node *> j_out_set(j_out.begin(), j_out.end());
                FilteredRobustPrune(node_j, j_out_set, a, R);   
            }
        }
    }
    return G;
}

void initialize_graph(Graph &G, const vector<vector<double>> &coords)
{
    // initialize nodes from coords
    for (size_t i = 0; i < coords.size(); i++) 
    {
        if (coords[i].empty()) {
            cerr << "Error: Empty coordinate vector at index " << i << endl;
            continue;
        }

        // first element is the label
        int label = static_cast<int>(coords[i][0]);

        // remaining elements are the coordinates
        vector<double> pointCoords(coords[i].begin() + 1, coords[i].end());

        // create a new Node
        Node* newNode = new Node(static_cast<int>(i), pointCoords, {}, label);

        // add the node to the graph
        G.addNode(newNode);
    }
}

unordered_map<int, int> compute_st_f(Graph &G, const set<int> &F)
{
    unordered_map<int, int> st_f; // st(f) for every f ∈ F

    // traverse through the labels in F
    for (int f : F)
    {
        // find the first node in the graph with label f
        vector<int> nodesWithLabel = G.findNodesWithLabel(f);

        if (!nodesWithLabel.empty())
        { // we assume that the first occurrence is the start node
            st_f[f] = nodesWithLabel[0];
        }
        else
        {
            cout << "No nodes found with label: " << f << endl;
        }
    }

    return st_f;
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