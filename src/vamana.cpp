#include "../include/vamana.hpp"
#include "../include/greedysearch.hpp"
#include "../include/robustprune.hpp"
#include "../include/generate_graph.hpp"
#include "../include/utility.hpp"
#include <algorithm>
#include <iostream>
#include <random>
#include <chrono>
#include <cassert>
#include <pthread.h>
#include "../include/utility.hpp"               // due to use of euclidean_distance_of_nodes and v

// coords = coords of all vectors in P dataset (Graph)
// randomPermutation = σ

// L is the set of closest neighbors returned from Greedy
// V is the set of visited nodes returned from Greedy
// sigma_i_out are the possible candidates, out-neighbors.

// calls findMedoid and prints time taken
// int calculate_medoid(vector<vector<double>> &coords)
// {
//     cout << "Start of medoid calculation..." << endl;

//     auto start = chrono::high_resolution_clock::now();

//     int medoidIndex = findMedoid(coords);

//     auto end = chrono::high_resolution_clock::now();
//     chrono::duration<double> duration = end - start;

//     cout << "Medoid calculation took: " << duration.count() << " seconds." << endl;

//     return medoidIndex;
// }

int Vamana(Graph &graph, vector<Node *> &coords, int R, double a, int int_L)
{
    vector<vector<double>> actual_coords;

    for (Node *node : coords) {
        actual_coords.push_back(node->getCoordinates());
    }

    generate_graph(graph, coords, R);

    int medoidNodeId = findMedoid(actual_coords);

    Node *medoid = graph.getNode(coords[medoidNodeId]->getId());

    if (medoid == nullptr)
    {
        cerr << "Error: Medoid node with ID " << medoidNodeId << " not found in the graph." << endl;
        return -1;
    }

    // make a random permutation of 1..n, to traverse the nodes in a random order
    vector<int> randomPermutation(coords.size());                // Size of vector randomPermutation = number of points in dataset = number of vectors in coords
    iota(randomPermutation.begin(), randomPermutation.end(), 0); // fills with numbers from 0 to coords.size() - 1

    // obtain a time-based seed:
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    shuffle(randomPermutation.begin(), randomPermutation.end(), default_random_engine(seed));

    for (int point_id : randomPermutation)
    {
        set<Node *> V_set;
        set<Node *> L_set;
        GreedySearch(medoid, actual_coords[point_id], 1, int_L, L_set, V_set);

        Node *sigma_i = graph.getNode(coords[point_id]->getId());
        if (sigma_i == nullptr)
        {
            cerr << "Error: Node with ID " << coords[point_id]->getId() << " not found in the graph." << endl;
            continue; // skip this iteration or handle the error appropriately
        }

        RobustPrune(sigma_i, V_set, a, R);

        list<Node *> sigma_i_out = sigma_i->getEdges();
        for (auto node_j : sigma_i_out)
        {
            list<Node *> j_out = node_j->getEdges();

            set<Node *> j_out_sigma_i(j_out.begin(), j_out.end());
            j_out_sigma_i.insert(sigma_i);

            if (static_cast<int>(j_out_sigma_i.size()) > R)
            {
                RobustPrune(node_j, j_out_sigma_i, a, R);
            }
            else
            {
                auto it = find(j_out.begin(), j_out.end(), sigma_i);
                if (it == j_out.end())
                { 
                    // sigma_i doesn't exist in j_out
                    node_j->mutex_lock(); // we are going to edit a node's list of neighbors, so we need to lock the mutex
                    node_j->addEdge(sigma_i);
                    node_j->mutex_unlock();
                }
            }
        }
    }

    return coords[medoidNodeId]->getId();
}

#define THREADS_NO 4

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
static void* childThread (void* args) {
    struct arguments *args_c = (struct arguments*)args;       // args_child

    for (auto it = args_c->it_begin; it != args_c->it_end; ++it) {           // [0. size_div_2)
        Node* node = it->second;
        int label = node->getLabel();

        for (auto it2 = it; it2 != args_c->nodes->end(); ++it2) {      // we start from it2 = it instead of it2 = it + 1, because we want to store that the distance of one node from itself is zero
            Node* node2 = it2->second;
            int label2 = node2->getLabel();

            // Skip if nodes don't have the same label
            if (label != label2) {
                continue;
                assert(0);
            }

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


int VamanaParallelDistances(Graph &graph, vector<Node *> &coords, int R, double a, int int_L)
{
    vector<vector<double>> actual_coords;

    for (Node *node : coords) {
        actual_coords.push_back(node->getCoordinates());
    }

    generate_graph_parallel(graph, coords, R);

    int medoidNodeId = findMedoid(actual_coords);
    // int medoidNodeId = parallel_findMedoid(actual_coords);
    // int medoidNodeId = parallel_2_findMedoid(actual_coords);

    Node *medoid = graph.getNode(coords[medoidNodeId]->getId());

    if (medoid == nullptr)
    {
        cerr << "Error: Medoid node with ID " << medoidNodeId << " not found in the graph." << endl;
        return -1;
    }

// Αποθήκευση αποστάσεων
    // Vector to hold THREADS_NO unordered_map objects
    vector<unordered_map<pair<Node*, Node*>, double, PairHash>> maps(THREADS_NO);        // we calculate & store only the distances between nodes of the same label 
    map<int, Node*> nodes = graph.getAdjList();

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

    // make a random permutation of 1..n, to traverse the nodes in a random order
    vector<int> randomPermutation(coords.size());                // Size of vector randomPermutation = number of points in dataset = number of vectors in coords
    iota(randomPermutation.begin(), randomPermutation.end(), 0); // fills with numbers from 0 to coords.size() - 1

    // obtain a time-based seed:
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    shuffle(randomPermutation.begin(), randomPermutation.end(), default_random_engine(seed));

    for (int point_id : randomPermutation)
    {
        set<Node *> V_set;
        set<Node *> L_set;

        Node *sigma_i = graph.getNode(coords[point_id]->getId());
        if (sigma_i == nullptr)
        {
            cerr << "Error: Node with ID " << coords[point_id]->getId() << " not found in the graph." << endl;
            continue; // skip this iteration or handle the error appropriately
        }

        GreedySearchIndex(medoid, sigma_i, 1, int_L, L_set, V_set, nodePairMap);
        RobustPrune(sigma_i, V_set, a, R);

        list<Node *> sigma_i_out = sigma_i->getEdges();
        for (auto node_j : sigma_i_out)
        {
            list<Node *> j_out = node_j->getEdges();

            set<Node *> j_out_sigma_i(j_out.begin(), j_out.end());
            j_out_sigma_i.insert(sigma_i);

            if (static_cast<int>(j_out_sigma_i.size()) > R)
            {
                RobustPrune(node_j, j_out_sigma_i, a, R);
            }
            else
            {
                auto it = find(j_out.begin(), j_out.end(), sigma_i);
                if (it == j_out.end())
                { 
                    // sigma_i doesn't exist in j_out
                    node_j->mutex_lock(); // we are going to edit a node's list of neighbors, so we need to lock the mutex
                    node_j->addEdge(sigma_i);
                    node_j->mutex_unlock();
                }
            }
        }
    }

    return coords[medoidNodeId]->getId();
}