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
#include <pthread.h>

#define NUM_THREADS 10

// parallel:

// struct Thread_params {
//     Graph* G;
//     vector<int>* randomPermutation;
//     int start_index;
//     int end_index;
//     map<int, Node*>* st_f;
//     int int_L;
//     double a;
//     int R;
//     pthread_mutex_t* mutex;
// };

// // thread function -- each thread will take on a chunk of nodes from randomPermutation and do the work of filteredVamana
// void *process_chunk(void *args) 
// {
//     Thread_params *params = static_cast<Thread_params *>(args);
//     map<int, Node *> &st_f = *(params->st_f);

//     for (int i = params->start_index; i < params->end_index; i++) 
//     {
//         int point_id = (*params->randomPermutation)[i];
//         pthread_mutex_lock(params->mutex);
//         Node* point = params->G->getNode(point_id);
//         int label = point->getLabel();
//         Node *start_node = st_f[label];
//         pthread_mutex_unlock(params->mutex);

//         set<Node *> L_set;
//         set<Node *> V_set;
//         set<Node *> S_set;

//         S_set.insert(start_node);
//         int k = 0;
//         set<int> F;
//         F.insert(label);

//         pthread_mutex_lock(params->mutex);
//         vector<double> coordinates = point->getCoordinates();
//         FilteredGreedySearch(S_set, coordinates, k, params->int_L, L_set, V_set, F);
//         pthread_mutex_unlock(params->mutex);

//         assert(L_set.size() == 0);

//         pthread_mutex_lock(params->mutex);
//         FilteredRobustPrune(point, V_set, params->a, params->R);
//         pthread_mutex_unlock(params->mutex);

//         list<Node *> point_out = point->getEdges();
//         for (auto j : point_out)
//         {   
//             pthread_mutex_lock(params->mutex);
//             j->addEdge(point); // addEdge won't add point as neighbor of j if that's already the case
//             list<Node *> j_out = j->getEdges();
//             pthread_mutex_unlock(params->mutex);

//             if (static_cast<int>(j_out.size()) > params->R)
//             {
//                 set<Node *> V_set(j_out.begin(), j_out.end());
//                 pthread_mutex_lock(params->mutex);
//                 FilteredRobustPrune(j, V_set, params->a, params->R);
//                 pthread_mutex_unlock(params->mutex);
//             }
//         }
//     }
//     return nullptr;
// }

// Graph filteredVamana(vector<vector<double>> &coords, double a, int int_L, int R, set<int> F, int taph, map<int, Node *> &st_f) 
// {
//     Graph G;

//     initialize_graph(G, coords);

//     st_f = FindMedoid(G, taph, F);

//     vector<int> randomPermutation(coords.size());
//     iota(randomPermutation.begin(), randomPermutation.end(), 0);            // starts from 0 until coords.size - 1
//     unsigned seed = chrono::system_clock::now().time_since_epoch().count(); // obtain a time-based seed and shuffle
//     shuffle(randomPermutation.begin(), randomPermutation.end(), default_random_engine(seed));
   
//     int chunk_size = (randomPermutation.size() + NUM_THREADS - 1) / NUM_THREADS; // round up to ensure all elements are processed

//     // initialize pthreads
//     pthread_t threads[NUM_THREADS];
//     Thread_params params[NUM_THREADS];
//     pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//     for (int i = 0; i < NUM_THREADS; i++)
//     {
//         // prepare thread parameters
//         int start = i * chunk_size;
//         int end = min(start + chunk_size, static_cast<int>(randomPermutation.size()));
//         params[i] = {&G, &randomPermutation, start, end, &st_f, int_L, a, R, &mutex};

//         // create a thread
//         if (pthread_create(&threads[i], nullptr, process_chunk, &params[i]) != 0) {
//             cerr << "Error creating thread for chunk no: " << i << endl;
//         }
//     }

//     // wait for all threads to complete
//     for (int j = 0; j < NUM_THREADS; j++) 
//     {
//         pthread_join(threads[j], nullptr);
//     }

//     pthread_mutex_destroy(&mutex);

//     return G;
// }

// serial:

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
//     unordered_map<int, set<int>> Fx = compute_Fx(G);

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

unordered_map<int, set<int>> compute_Fx(Graph &G)
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