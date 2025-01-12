/* FilteredVamana parallel implementation with parallel distance calculation using pthreads */

#include "../include/filteredVamanaParallelDistances.hpp"
#include "../include/filteredGreedySearch.hpp"
#include "../include/filteredrobustprune.hpp"
#include "../include/FindMedoid.hpp"
#include "../include/generate_graph.hpp"
#include "../include/utility.hpp"               // due to use of euclidean_distance_of_nodes and v
#include <algorithm>
#include <iostream>
#include <random>
#include <chrono>
#include <cassert>
#include <pthread.h>

#define NUM_THREADS 10
#define THREADS_NO 10

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
    unordered_map<pair<Node*, Node*>, double, PairHash> *nodePairMap;
};

// thread function -- each thread will take on a chunk of nodes from randomPermutation and do the work of filteredVamana
void *processChunk(void *args) 
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
        FilteredGreedySearchIndex(S_set, point, k, params->int_L, L_set, V_set, F, *(params->nodePairMap));
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

// thread parameters for distance calculation
struct arguments {
    map<int, Node*> *nodes;
    map<int, Node*>::iterator it_begin;
    map<int, Node*>::iterator it_end;
    unordered_map<pair<Node*, Node*>, double, PairHash> *nodePairMap;
};

// Συνάρτηση που υπολογίζει τις παραμέτρους που θα λάβει κάθε thread που τρέχει την συνάρτηση childThread
static void fill_up_args(map<int, Node*> &nodes, vector<unordered_map<pair<Node*, Node*>, double, PairHash>> &maps, int threads_no, struct arguments *array) {

    int size = nodes.size();
    int chunk = size / threads_no;  // (πόσα nodes έχει κάθε thread)
    if(size % threads_no != 0)
        chunk++;

    int has_done = 0;
    for(int i = 0; i < threads_no; i++) {
        int offset = chunk;     // 4
        if(has_done + offset > size)
            offset = size - has_done;


        if(i == 0)
            array[i].it_begin = nodes.begin();
        else {
            array[i].it_begin = array[i-1].it_end;
        }

        array[i].it_end = array[i].it_begin;
        advance(array[i].it_end, offset);
        has_done += offset;

        array[i].nodes = &nodes;
        array[i].nodePairMap = &(maps[i]);
    }
    return;
}

// Συνάρτηση που τρέχει ένα thread που υπολογίζει τις αποστάσεις. Λαμβάνει ως παράμετρο ένα struct arguments*
void* childThread (void* args) {
    struct arguments *args_c = (struct arguments*)args;       // args_child

    for (auto it = args_c->it_begin; it != args_c->it_end; ++it) {           // [0. size_div_2)
        Node* node = it->second;
        int label = node->getLabel();

        for (auto it2 = it; it2 != args_c->nodes->end(); ++it2) {      // we start from it2 = it instead of it2 = it + 1, because we want to store that the distance of one node from itself is zero
            Node* node2 = it2->second;
            int label2 = node2->getLabel();

            // Skip if nodes don't have the same label
            if (label != label2)
                continue;

            // Create an order-independent key
            pair<Node*, Node*> key = {min(node, node2), max(node, node2)};      // The hash and key use min and max to ensure that {node1, node2} is treated the same as {node2, node1}. This eliminates the need to insert the symmetric pair manually.
            double dist = euclidean_distance_of_nodes(node, node2);

            // Insert if the key does not exist
            assert(args_c->nodePairMap->find(key) == args_c->nodePairMap->end());     // shouldn't exist already        
            (*args_c->nodePairMap)[key] = dist; // Insert the key-value pair
        }
    }

    pthread_exit(NULL);
}


Graph filteredVamanaParallelDistances(vector<vector<double>> &coords, double a, int int_L, int R, set<int> F, int taph, map<int, Node *> &st_f) 
{
    Graph G;

    initialize_graph_parallel(G, coords);
    // generate_random_edges(G, 1);

    st_f = FindMedoid(G, taph, F);

// Αποθήκευση αποστάσεων
    // Vector to hold THREADS_NO unordered_map objects
    vector<unordered_map<pair<Node*, Node*>, double, PairHash>> maps(THREADS_NO);        // we calculate & store only the distances between nodes of the same label 
    map<int, Node*> nodes = G.getAdjList();

    pthread_t thread_id[THREADS_NO];        // Store the threadIDs in an array
    struct arguments array[THREADS_NO];     // Store the struct arguments of each thread in an array
    fill_up_args(nodes, maps, THREADS_NO, array);            // Prepare arguments for each thread

    // Create a new thread. The new thread will run the childThread function.
    for (int threadCount = 0; threadCount < THREADS_NO; threadCount++){
        pthread_create (&thread_id[threadCount], NULL, &childThread, &(array[threadCount]));
    }

	for (int threadCount = 0; threadCount< THREADS_NO; threadCount++) {    // Join after all thread creation
    	void *status;
		pthread_join(thread_id[threadCount], &status);
    }	

    // Combine all maps into one
    unordered_map<pair<Node*, Node*>, double, PairHash> nodePairMap;
    for (const auto& map : maps) {
        nodePairMap.insert(map.begin(), map.end());
    }
    // Size of nodePairMap should be 11029349 (without the zero distances added)
// Τέλος αποθήκευσης αποστάσεων

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
        params[i] = {&G, &randomPermutation, start, end, &st_f, int_L, a, R, &mutex, &nodePairMap};

        // create a thread
        if (pthread_create(&threads[i], nullptr, processChunk, &params[i]) != 0) {
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