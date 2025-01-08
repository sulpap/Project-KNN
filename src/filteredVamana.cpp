#include "../include/filteredVamana.hpp"
#include "../include/filteredGreedySearch.hpp"
#include "../include/filteredrobustprune.hpp"
#include "../include/FindMedoid.hpp"
#include "../include/generate_graph.hpp"
#include "../include/utility.hpp"               // due to use of euclidean_distance_of_nodes
#include <algorithm>
#include <iostream>
#include <random>
#include <chrono>
#include <cassert>

#define THREADS_NO 5
pthread_mutex_t mutex_c = PTHREAD_MUTEX_INITIALIZER;        // mutex to protect edits to nodePairMap (is used by threads that calculate distances)

// Παράμετροι που θα λάβει ένα thread για τον υπολογισμό των αποστάσεων
struct arguments {
    map<int, Node*> *nodes;
    map<int, Node*>::iterator it_begin;
    map<int, Node*>::iterator it_end;
    unordered_map<pair<Node*, Node*>, double, PairHash> *nodePairMap;
};

// Συνάρτηση που υπολογίζει τις παραμέτρους που θα λάβει κάθε thread που τρέχει την συνάρτηση childThread
static void fill_up_args(map<int, Node*> &nodes, unordered_map<pair<Node*, Node*>, double, PairHash> &nodePairMap, int threads_no, struct arguments *array) {

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
        array[i].nodePairMap = &nodePairMap;
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
            pthread_mutex_lock(&mutex_c);               // size of nodePairMap changes in runtime
            assert(args_c->nodePairMap->find(key) == args_c->nodePairMap->end());     // shouldn't exist already        
            (*args_c->nodePairMap)[key] = dist; // Insert the key-value pair
            pthread_mutex_unlock (&mutex_c);
        }
    }

    pthread_exit(NULL);
}

Graph filteredVamana(vector<vector<double>> &coords, double a, int int_L, int R, set<int> F, int taph, map<int, Node *> &st_f)
{
    // 1. Initialize an empty graph
    Graph G;

    // fill the graph with nodes based on the coords info (label & point coords)
    initialize_graph(G, coords);

    // 2. Let st(f) denote the start node for filter label f for every f ∈ F
    st_f = FindMedoid(G, taph, F);

    unordered_map<pair<Node*, Node*>, double, PairHash> nodePairMap;      // we calculate & store only the distances between nodes of the same label
    map<int, Node*> nodes = G.getAdjList();

    pthread_t thread_id[THREADS_NO];        // Store the threadIDs in an array
    struct arguments array[THREADS_NO];     // Store the struct arguments of each thread in an array
    fill_up_args(nodes, nodePairMap, THREADS_NO, array);            // Prepare arguments for each thread

    // Create a new thread. The new thread will run the childThread function.
    for (int threadCount = 0; threadCount < THREADS_NO; threadCount++){
        pthread_create (&thread_id[threadCount], NULL, &childThread, &(array[threadCount]));
		printf("Parent has created thread %d with thread_id %u \n", threadCount, (unsigned int)thread_id[threadCount]);

    }
	printf("Parent has CREATED ALL threads.\n");

	for (int threadCount = 0; threadCount< THREADS_NO; threadCount++) {    // Join after all thread creation
    	void *status;
		pthread_join(thread_id[threadCount], &status);
		printf("Parent: completed join with thread %d having a status of %ld\n",threadCount,(long)status);
    }
	printf("Parent has WAITED FOR ALL threads to finish.\n");	

    cout << "Size of nodePairMap = " << nodePairMap.size() << endl;     // should be 11029349 (without the zero distances added)

    // 3. Let σ (randomPermutation) be a random permutation of [n]
    vector<int> randomPermutation(coords.size());
    iota(randomPermutation.begin(), randomPermutation.end(), 0);            // starts from 0 μέχρι until coords.size - 1
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

// cout << "fileterd, start node id: " << start_node->getId() << " and label: " << label << " loop: " << point_id << endl;

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

// // database P is basically coords
// // we declare label set f for each node, even though the nodes have only one label, so the set always contains of one element.

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