#include "../include/stitchedVamana.hpp"
#include "../include/vamana.hpp"
#include "../include/filteredrobustprune.hpp"
#include <pthread.h>

// in stitchedVamana, we know the set of points beforehand
// stitchedVamana returns a stitched graph, that consists of a collection of subgraphs, one for each label.
// if there are no nodes for a label, then the graph would be empty.

using namespace std;

// Graph stitchedVamana(vector<vector<double>> &coords, set<int> F, double a, int L_small, int R_small, int R_stitched, map<int, Node *> &medoids) 
// {
//     // 1. Initialize G = (V, E) to an empty graph
//     Graph G;

//     // 2. Let Fx ⊆ F be the label-set for every x ∈ P
//     unordered_map<int, set<int>> Fx = compute_Fx(coords); // Fx maps node IDs to their label sets

//     // 3. Let Pf ⊆ P be the set of points with label f ∈ F
//     unordered_map<int, vector<Node *>> PfMap = compute_PfMap(coords, F); // Map each label f to its corresponding set of points (coordinates)

//     // foreach f ∈ F do
//     for (int f : F) 
//     {
//         // get Pf (the nodes) for this label
//         vector<Node *> Pf = PfMap[f];

//         // if Pf is empty, then we were given no nodes with this label
//         if (Pf.empty()) 
//         {
//             // continue without calling vamana to avoid extra work
//             continue;
//         }

//         // 4. Let Gf ← Vamana(Pf, α, R_small, L_small)
//         Graph Gf;

//         int medoidId = Vamana(Gf, Pf, R_small, a, L_small); // we also give f, because vamana creates the graph, so it
//                                                                // needs to add the values of the nodes, in which is the label

//         // merge "stitch" graphs
//         G.graphUnion(move(Gf));

//         Gf.clear(); // Gf's adj list is already cleared by graphUnion. clear the nodes here.

//         // store the medoid node
//         store_medoid(G, medoids, f, medoidId);
//     }

//     // foreach v ∈ V do
//     // for (auto &[nodeId, nodePtr] : G.getAdjList()) {
//     //     // 5. FilteredRobustPrune(v, N_out(v), α, R_stitched)
//     //     list<Node *> temp = nodePtr->getEdges();
//     //     set<Node *> N_out(temp.begin(), temp.end());
//     //     FilteredRobustPrune(nodePtr, N_out, a, R_stitched);
//     // }

//     return G;
// }

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

unordered_map<int, vector<Node *>> compute_PfMap(vector<vector<double>> &coords, set<int> F) 
{
    unordered_map<int, vector<Node *>> PfMap;
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

            Node *tempNode = new Node(i, pointCoords, {}, label);

            // add the node to the corresponding label in PfMap
            PfMap[label].push_back(tempNode);
        }
    }
    return PfMap;
}

void store_medoid(Graph &G, map<int, Node *> &medoids, int f, int medoidId)
{
    // find the medoid Node in Gf using its ID
    Node *medoidNode = G.getNode(medoidId);
    if (medoidNode != nullptr) {
        // store the medoid: key is the label (f), value is the Node*
        medoids[f] = medoidNode;
    } else {
        cerr << "Error: Medoid ID " << medoidId << " not found in graph Gf for label " << f << endl;
    }
}

//parallel:

struct Thread_params {
    int label;
    vector<Node *> Pf;
    double a;                    
    int R_small;                 
    int L_small;
    Graph *graph;                // pointer to the main graph
    map<int, Node *> *medoids;   // pointer to medoids map
    pthread_mutex_t *mutex;     // we will need the mutex for the functions that are accessed by the threads (graphUnion and store_medoid)
};

// thread function -- is done by each thread: the work of processing one label
void *processLabel(void *args) 
{
    Thread_params *params = static_cast<Thread_params *>(args);

    // create a subgraph for this label
    Graph Gf;
    int medoidId = Vamana(Gf, params->Pf, params->R_small, params->a, params->L_small);

    // merge (stitch) the subgraph into the main graph
    pthread_mutex_lock(params->mutex);
    params->graph->graphUnion(std::move(Gf));
    pthread_mutex_unlock(params->mutex); 

    Gf.clear();

    // store the medoid node
    if (medoidId != -1) 
    {
        pthread_mutex_lock(params->mutex);
        store_medoid(*params->graph, *params->medoids, params->label, medoidId);
        pthread_mutex_unlock(params->mutex);
    }

    return nullptr;
}

Graph stitchedVamana(vector<vector<double>> &coords, set<int> F, double a, int L_small, int R_small, int R_stitched, map<int, Node *> &medoids) 
{
    Graph G;
    unordered_map<int, set<int>> Fx = compute_Fx(coords);
    unordered_map<int, vector<Node *>> PfMap = compute_PfMap(coords, F);

    // initialize pthreads
    pthread_t threads[F.size()];
    Thread_params params[F.size()];
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    int i = 0;
    for (int f : F) 
    {
        vector<Node *> Pf = PfMap[f];

        // if Pf is empty, then we were given no nodes with this label
        if (Pf.empty()) 
        {
            // continue without calling vamana to avoid extra work
            continue;
        }

        // prepare thread parameters
        params[i] = {f, Pf, a, R_small, L_small, &G, &medoids, &mutex};

        // create a thread
        if (pthread_create(&threads[i], nullptr, processLabel, &params[i]) != 0) {
            cerr << "Error creating thread for label " << f << endl;
        }

        i++;
    }

    // wait for all threads to complete
    for (int j = 0; j < i; j++) 
    {
        pthread_join(threads[j], nullptr);
    }

    pthread_mutex_destroy(&mutex);

    return G;
}
