/* StitchedVamana parallel implementation file:
   Contains the parallel implementation of stitchedVamana with pthreads.

   stitchedVamana returns a stitched graph, that consists of a collection of subgraphs, one for each label.
   if there are no nodes for a label, then the graph would be empty. 
*/

#include "../include/stitchedVamana.hpp"
#include "../include/stitchedVamanaParallel.hpp"
#include "../include/vamana.hpp"
#include "../include/filteredrobustprune.hpp"
#include "../include/generate_graph.hpp"
#include <pthread.h>

using namespace std;

struct Thread_params {
    int label;
    vector<Node *> Pf;
    double a;                    
    int R_small;                 
    int L_small;
    Graph *graph;                   // pointer to the main graph
    map<int, Node *> *medoids;      // pointer to medoids map
    pthread_mutex_t *mutex_union;   // mutex for graphUnion - we will need the mutexes for the functions that are accessed by the threads (shared functions)
    pthread_mutex_t *mutex_medoid;  // mutex for store_medoid
};

// thread function -- is done by each thread: the work of processing one label: make Gf, stitch it, store medoid.
static void *processLabel(void *args) 
{
    Thread_params *params = static_cast<Thread_params *>(args);

    // create a subgraph for this label
    Graph Gf;
    int medoidId = VamanaParallelDistances(Gf, params->Pf, params->R_small, params->a, params->L_small);

    // merge (stitch) the subgraph into the main graph
    pthread_mutex_lock(params->mutex_union);
    params->graph->graphUnion(std::move(Gf));
    pthread_mutex_unlock(params->mutex_union); 

    Gf.clear();

    // store the medoid node
    if (medoidId != -1) 
    {
        pthread_mutex_lock(params->mutex_medoid);
        store_medoid(*params->graph, *params->medoids, params->label, medoidId);
        pthread_mutex_unlock(params->mutex_medoid);
    }

    return nullptr;
}


Graph stitchedVamanaParallelDistances(vector<vector<double>> &coords, set<int> F, double a, int L_small, int R_small, int R_stitched, map<int, Node *> &medoids) 
{
    Graph G;
    unordered_map<int, set<int>> Fx = compute_Fx(coords);
    unordered_map<int, vector<Node *>> PfMap = compute_PfMap(coords, F);

    // initialize pthreads
    pthread_t threads[F.size()];
    Thread_params params[F.size()];
    pthread_mutex_t mutex_union = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t mutex_medoid = PTHREAD_MUTEX_INITIALIZER;

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
        params[i] = {f, Pf, a, R_small, L_small, &G, &medoids, &mutex_union, &mutex_medoid};

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

    pthread_mutex_destroy(&mutex_union);
    pthread_mutex_destroy(&mutex_medoid);

    // connect_subgraphs(G, PfMap);

    return G;
}