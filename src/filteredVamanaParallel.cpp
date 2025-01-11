/* FilteredVamana parallel implementation using pthreads */

#include "../include/filteredVamanaParallel.hpp"
#include "../include/filteredGreedySearch.hpp"
#include "../include/filteredrobustprune.hpp"
#include "../include/FindMedoid.hpp"
#include "../include/generate_graph.hpp"
#include "../include/utility.hpp" 
#include <algorithm>
#include <iostream>
#include <random>
#include <chrono>
#include <cassert>
#include <pthread.h>

#define NUM_THREADS 10

struct Thread_params {
    Graph* G;
    vector<int>* randomPermutation;
    int start_index;
    int end_index;
    map<int, Node*>* st_f;
    int int_L;
    double a;
    int R;
    pthread_mutex_t* mutex;
};

// thread function -- each thread will take on a chunk of nodes from randomPermutation and do the work of filteredVamana
void *process_chunk(void *args) 
{
    Thread_params *params = static_cast<Thread_params *>(args);
    map<int, Node *> &st_f = *(params->st_f);

    for (int i = params->start_index; i < params->end_index; i++) 
    {
        int point_id = (*params->randomPermutation)[i];
        pthread_mutex_lock(params->mutex);
        Node* point = params->G->getNode(point_id);
        int label = point->getLabel();
        Node *start_node = st_f[label];
        pthread_mutex_unlock(params->mutex);

        set<Node *> L_set;
        set<Node *> V_set;
        set<Node *> S_set;

        S_set.insert(start_node);
        int k = 0;
        set<int> F;
        F.insert(label);

        pthread_mutex_lock(params->mutex);
        vector<double> coordinates = point->getCoordinates();
        FilteredGreedySearch(S_set, coordinates, k, params->int_L, L_set, V_set, F);
        pthread_mutex_unlock(params->mutex);

        assert(L_set.size() == 0);

        pthread_mutex_lock(params->mutex);
        FilteredRobustPrune(point, V_set, params->a, params->R);
        pthread_mutex_unlock(params->mutex);

        list<Node *> point_out = point->getEdges();
        for (auto j : point_out)
        {   
            pthread_mutex_lock(params->mutex);
            j->addEdge(point); // addEdge won't add point as neighbor of j if that's already the case
            list<Node *> j_out = j->getEdges();
            pthread_mutex_unlock(params->mutex);

            if (static_cast<int>(j_out.size()) > params->R)
            {
                set<Node *> V_set(j_out.begin(), j_out.end());
                pthread_mutex_lock(params->mutex);
                FilteredRobustPrune(j, V_set, params->a, params->R);
                pthread_mutex_unlock(params->mutex);
            }
        }
    }
    return nullptr;
}

Graph filteredVamanaParallel(vector<vector<double>> &coords, double a, int int_L, int R, set<int> F, int taph, map<int, Node *> &st_f) 
{
    Graph G;

    initialize_graph_parallel(G, coords);
    // generate_random_edges(G, 1);

    st_f = FindMedoid(G, taph, F);

    vector<int> randomPermutation(coords.size());
    iota(randomPermutation.begin(), randomPermutation.end(), 0);            // starts from 0 until coords.size - 1
    unsigned seed = chrono::system_clock::now().time_since_epoch().count(); // obtain a time-based seed and shuffle
    shuffle(randomPermutation.begin(), randomPermutation.end(), default_random_engine(seed));
   
    int chunk_size = (randomPermutation.size() + NUM_THREADS - 1) / NUM_THREADS; // round up to ensure all elements are processed

    // initialize pthreads
    pthread_t threads[NUM_THREADS];
    Thread_params params[NUM_THREADS];
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    for (int i = 0; i < NUM_THREADS; i++)
    {
        // prepare thread parameters
        int start = i * chunk_size;
        int end = min(start + chunk_size, static_cast<int>(randomPermutation.size()));
        params[i] = {&G, &randomPermutation, start, end, &st_f, int_L, a, R, &mutex};

        // create a thread
        if (pthread_create(&threads[i], nullptr, process_chunk, &params[i]) != 0) {
            cerr << "Error creating thread for chunk no: " << i << endl;
        }
    }

    // wait for all threads to complete
    for (int j = 0; j < NUM_THREADS; j++) 
    {
        pthread_join(threads[j], nullptr);
    }

    pthread_mutex_destroy(&mutex);

    return G;
}
