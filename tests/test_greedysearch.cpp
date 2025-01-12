#include "catch.hpp"
#include "../include/graph.hpp"
#include "../include/greedysearch.hpp"
#include "../include/fvecs_read.hpp"
#include "../include/ivecs_read.hpp"
#include "../include/utility.hpp"

TEST_CASE("GreedySearch in small connected graph") {
    Graph graph;

    // list initialization syntax available for C++11 and newer
    // {1.0, 1.0} initializes the std::vector<double> with two elements.
    // {} creates an empty std::list<Node*>
    Node n0 = Node(0, {1.0, 1.0}, {}, {});
    Node n1 = Node(1, {1.0, 3.0}, {}, {});
    Node n2 = Node(2, {2.0, 2.0}, {}, {});

    graph.addNode(&n0);
    graph.addNode(&n1);
    graph.addNode(&n2);

    graph.addEdge(0, 1);
    graph.addEdge(1, 0);
    graph.addEdge(0, 2);
    graph.addEdge(2, 0);
    graph.addEdge(1, 2);
    graph.addEdge(2, 1);
 
    set<Node*> L_set;
    set<Node*> V_set;
    vector<double> query = {2.0, 2.1};

    set<Node*> set_test;
    set_test.insert(&n0);
    set_test.insert(&n1);
    set_test.insert(&n2);

// 1st run k = 1, L = 3 (L = number of nodes in graph):
    GreedySearch(&n0, query, 1, 3, L_set, V_set);
    REQUIRE(L_set.size() == 1);
    
    auto it = L_set.begin();        // iterator points to a Node*
    REQUIRE((*it)->getId() == 2);   // node 2 should be closest neighbor
    REQUIRE(V_set == set_test);     // test of V_set

// 2nd run k = L = 3 (L = number of nodes in graph):
    L_set.clear();                  // L_set, V_set should be empty before new call of greedy
    V_set.clear();
    GreedySearch(&n0, query, 3, 3, L_set, V_set);
    REQUIRE(L_set.size() == 3);
    REQUIRE(L_set == set_test);
    REQUIRE(V_set == set_test);

// 3d run k = 1, L = 2 (L < number of nodes in graph):
    L_set.clear();
    V_set.clear();
    GreedySearch(&n0, query, 1, 2, L_set, V_set);
    REQUIRE(L_set.size() == 1);

    it = L_set.begin();             // iterator points to a Node*
    REQUIRE((*it)->getId() == 2);   // node 2 should be closest neighbor
    
    REQUIRE(V_set == set_test);

// 4th run k = 1, L = 5 (L > number of nodes in graph):
    L_set.clear();
    V_set.clear();
    GreedySearch(&n0, query, 1, 5, L_set, V_set);
    REQUIRE(L_set.size() == 1);

    it = L_set.begin();             // iterator points to a Node*
    REQUIRE((*it)->getId() == 2);   // node 2 should be closest neighbor
    
    REQUIRE(V_set == set_test);
}

TEST_CASE("GreedySearch in fully disconnected small graph") {
    Graph graph;

    Node n0 = Node(0, {1.0, 1.0}, {}, {});
    Node n1 = Node(1, {1.0, 3.0}, {}, {});
    Node n2 = Node(2, {2.0, 2.0}, {}, {});

    graph.addNode(&n0);
    graph.addNode(&n1);
    graph.addNode(&n2);

    set<Node*> L_set;
    set<Node*> V_set;
    vector<double> query = {2.0, 2.1};

// 1st run k = 1, L = 3
    GreedySearch(&n0, query, 1, 3, L_set, V_set);
    REQUIRE(L_set.size() == 1);

    auto it = L_set.begin();        // iterator points to a Node*
    REQUIRE((*it)->getId() == 0);   // node 0 should be closest neighbor

    it = V_set.begin();
    REQUIRE((*it)->getId() == 0);

// 2nd run k = 2, L = 3 (should have same results with previous run)
    L_set.clear();
    V_set.clear();

    GreedySearch(&n0, query, 2, 3, L_set, V_set);
    REQUIRE(L_set.size() == 1);     // one neighbor! not 2!

    it = L_set.begin();             // iterator points to a Node*
    REQUIRE((*it)->getId() == 0);   // node 0 should be closest neighbor

    it = V_set.begin();
    REQUIRE((*it)->getId() == 0);
}


TEST_CASE("GreedySearch in fully disconnected big graph") {
// 1. Read all vectors from the fvecs file
    vector<vector<float>> fvecs_f_base = fvecs_read("datasets/siftsmall/siftsmall_base.fvecs", 1, 10000);
    vector<vector<double>> fvecs_d_base = convert_to_double(fvecs_f_base); 
    int fvecs_d_base_size = fvecs_d_base.size();
    
// 2. Δημιουργία fully disconnected graph
    Graph graph;
    // 2.1 Προσθήκη nodes στον γράφο

    // In nodes, we store pointers of each Node
    vector<Node*> nodes(fvecs_d_base_size);

    for(int i = 0; i < fvecs_d_base_size; i++) {
        Node* node_ptr = new Node(i, fvecs_d_base[i], {}, {});
        nodes[i] = node_ptr;
        graph.addNode(node_ptr);
    }

// 3. Get query
    // We get one query of the dataset (we get the first query)
    vector<vector<float>> fvecs_f_queries = fvecs_read("datasets/siftsmall/siftsmall_query.fvecs", 1, 1);
    vector<float> query_f = fvecs_f_queries[0];
    vector<double> query(query_f.begin(), query_f.end());
 
// 4. Run greedy
    set<Node*> L_set;
    set<Node*> V_set;
    // We set as start node, node with id = 0
    GreedySearch(nodes[0], query, 1, 10000, L_set, V_set);

// 5. Compare results
    REQUIRE(L_set.size() == 1);
    REQUIRE(V_set.size() == 1);

    auto it = L_set.begin(); 
    REQUIRE(*it == nodes[0]);

// 6. Clear up
    for(int i = 0; i < fvecs_d_base_size; i++) {
        delete nodes[i];
    }
}


#define UPPER_BOUND 10000
// *!DON'T DELETE!* It's commented, because it takes a long time to finish due contruction of graph (not due to greedy!)

// TEST_CASE("GreedySearch in fully connected big graph") {
// // 1. Read all vectors from the fvecs file
//     vector<vector<float>> fvecs_f_base = fvecs_read("datasets/siftsmall/siftsmall_base.fvecs", 1, UPPER_BOUND);
//     vector<vector<double>> fvecs_d_base = convert_to_double(fvecs_f_base); 
//     int fvecs_d_base_size = fvecs_d_base.size();
    
// // 2. Δημιουργία fully connected graph
//     Graph graph;
//     // 2.1 Προσθήκη nodes στον γράφο

//     // In nodes, we store pointers of each Node
//     vector<Node*> nodes(fvecs_d_base_size);

//     for(int i = 0; i < fvecs_d_base_size; i++) {
//         Node* node_ptr = new Node(i, fvecs_d_base[i], {});
//         nodes[i] = node_ptr;
//         graph.addNode(node_ptr);
//     }

//     // 2.2. Create fully connected graph (κάθε κόμβος συνδέεται με κάθε κόμβο)
//     for(int i = 0; i < fvecs_d_base_size; i++) {
//         for(int j = 0; j < fvecs_d_base_size; j++) {
//             if(i != j) {    // Don't insert an edge from yourself to yourself
//                 graph.addEdge(i, j);
//             }
//         }
//     }

// // 3. Get query
//     // We get one query of the dataset (we get the first query)
//     vector<vector<float>> fvecs_f_queries = fvecs_read("datasets/siftsmall/siftsmall_query.fvecs", 1, 1);
//     vector<float> query_f = fvecs_f_queries[0];
//     vector<double> query(query_f.begin(), query_f.end());
 
// // 4. Run greedy
//     set<Node*> L_set;
//     set<Node*> V_set;
//     // We set as start node, node with id = 0
//     GreedySearch(nodes[0], query, 1, UPPER_BOUND, L_set, V_set);

//     REQUIRE(L_set.size() == 1);
//     REQUIRE(V_set.size() == UPPER_BOUND);

//     auto it = L_set.begin(); 
//     int greedy_sol = (*it)->getId();

// // 5. Compare results with ground truth

//     // We take only the ground truth of the first query
//     // Therefore, we retrieve the 1st vector of ivecs file
//     vector<vector<int>> ivecs_gt = ivecs_read("datasets/siftsmall/siftsmall_groundtruth.ivecs", 1, 1);
//     vector<int> ground_truth = ivecs_gt[0];

//     int gt_sol = ground_truth[0];

//     REQUIRE(greedy_sol == gt_sol);

// // 6. Clear up
//     for(int i = 0; i < fvecs_d_base_size; i++) {
//         delete nodes[i];
//     }
// }

TEST_CASE("GreedySearchIndex in small connected graph") {
    Graph graph;

    // list initialization syntax available for C++11 and newer
    // {1.0, 1.0} initializes the std::vector<double> with two elements.
    // {} creates an empty std::list<Node*>
    Node n0 = Node(0, {1.0, 1.0}, {}, {});
    Node n1 = Node(1, {1.0, 3.0}, {}, {});
    Node n2 = Node(2, {2.0, 2.0}, {}, {});

    graph.addNode(&n0);
    graph.addNode(&n1);
    graph.addNode(&n2);

    graph.addEdge(0, 1);
    graph.addEdge(1, 0);
    graph.addEdge(0, 2);
    graph.addEdge(2, 0);
    graph.addEdge(1, 2);
    graph.addEdge(2, 1);
 
    set<Node*> L_set;
    set<Node*> V_set;
    Node query_node = Node(3, {2.0, 2.1}, {}, {});
    graph.addNode(&query_node);         // query_node should be in graph for greedyIndex to work

    set<Node*> set_test;
    set_test.insert(&n0);
    set_test.insert(&n1);
    set_test.insert(&n2);

    // Fill up nodePairSum
    map<int, Node*> nodes = graph.getAdjList();
    unordered_map<pair<Node*, Node*>, double, PairHash> nodePairMap;        // Create an order-independent key
    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        Node* node = it->second;
        int label = node->getLabel();

        for (auto it2 = it; it2 != nodes.end(); ++it2) {      // we start from it2 = it instead of it2 = it + 1, because we want to store that the distance of one node from itself is zero
            Node* node2 = it2->second;
            int label2 = node2->getLabel();

            // Skip if nodes don't have the same label
            if (label != label2)
                continue;

            // Create an order-independent key
            pair<Node*, Node*> key = {min(node, node2), max(node, node2)};      // The hash and key use min and max to ensure that {node1, node2} is treated the same as {node2, node1}. This eliminates the need to insert the symmetric pair manually.
            double dist = euclidean_distance_of_nodes(node, node2);

            // Insert if the key does not exist
            assert(nodePairMap.find(key) == nodePairMap.end());       // shouldn't exist already        
            nodePairMap[key] = dist;                                                    // Insert the key-value pair
        }
    }

// 1st run k = 1, L = 3 (L = number of nodes in graph):
    GreedySearchIndex(&n0, &query_node, 1, 3, L_set, V_set, nodePairMap);
    REQUIRE(L_set.size() == 1);
    
    auto it = L_set.begin();        // iterator points to a Node*
    REQUIRE((*it)->getId() == 2);   // node 2 should be closest neighbor
    REQUIRE(V_set == set_test);     // test of V_set

// 2nd run k = L = 3 (L = number of nodes in graph):
    L_set.clear();                  // L_set, V_set should be empty before new call of greedy
    V_set.clear();
    GreedySearchIndex(&n0, &query_node, 3, 3, L_set, V_set, nodePairMap);
    REQUIRE(L_set.size() == 3);
    REQUIRE(L_set == set_test);
    REQUIRE(V_set == set_test);

// 3d run k = 1, L = 2 (L < number of nodes in graph):
    L_set.clear();
    V_set.clear();
    GreedySearchIndex(&n0, &query_node, 1, 2, L_set, V_set, nodePairMap);
    REQUIRE(L_set.size() == 1);

    it = L_set.begin();             // iterator points to a Node*
    REQUIRE((*it)->getId() == 2);   // node 2 should be closest neighbor
    
    REQUIRE(V_set == set_test);

// 4th run k = 1, L = 5 (L > number of nodes in graph):
    L_set.clear();
    V_set.clear();
    GreedySearchIndex(&n0, &query_node, 1, 5, L_set, V_set, nodePairMap);
    REQUIRE(L_set.size() == 1);

    it = L_set.begin();             // iterator points to a Node*
    REQUIRE((*it)->getId() == 2);   // node 2 should be closest neighbor
    
    REQUIRE(V_set == set_test);
}

TEST_CASE("GreedySearchIndex in fully disconnected small graph") {
    Graph graph;

    Node n0 = Node(0, {1.0, 1.0}, {}, {});
    Node n1 = Node(1, {1.0, 3.0}, {}, {});
    Node n2 = Node(2, {2.0, 2.0}, {}, {});

    graph.addNode(&n0);
    graph.addNode(&n1);
    graph.addNode(&n2);

    set<Node*> L_set;
    set<Node*> V_set;
    Node query_node = Node(3, {2.0, 2.1}, {}, {});
    graph.addNode(&query_node);         // query_node should be in graph for greedyIndex to work

    // Fill up nodePairSum
    map<int, Node*> nodes = graph.getAdjList();
    unordered_map<pair<Node*, Node*>, double, PairHash> nodePairMap;        // Create an order-independent key
    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        Node* node = it->second;
        int label = node->getLabel();

        for (auto it2 = it; it2 != nodes.end(); ++it2) {      // we start from it2 = it instead of it2 = it + 1, because we want to store that the distance of one node from itself is zero
            Node* node2 = it2->second;
            int label2 = node2->getLabel();

            // Skip if nodes don't have the same label
            if (label != label2)
                continue;

            // Create an order-independent key
            pair<Node*, Node*> key = {min(node, node2), max(node, node2)};      // The hash and key use min and max to ensure that {node1, node2} is treated the same as {node2, node1}. This eliminates the need to insert the symmetric pair manually.
            double dist = euclidean_distance_of_nodes(node, node2);

            // Insert if the key does not exist
            assert(nodePairMap.find(key) == nodePairMap.end());       // shouldn't exist already        
            nodePairMap[key] = dist;                                                    // Insert the key-value pair
        }
    }

// 1st run k = 1, L = 3
    GreedySearchIndex(&n0, &query_node, 1, 3, L_set, V_set, nodePairMap);
    REQUIRE(L_set.size() == 1);

    auto it = L_set.begin();        // iterator points to a Node*
    REQUIRE((*it)->getId() == 0);   // node 0 should be closest neighbor

    it = V_set.begin();
    REQUIRE((*it)->getId() == 0);

// 2nd run k = 2, L = 3 (should have same results with previous run)
    L_set.clear();
    V_set.clear();

    GreedySearchIndex(&n0, &query_node, 2, 3, L_set, V_set, nodePairMap);
    REQUIRE(L_set.size() == 1);     // one neighbor! not 2!

    it = L_set.begin();             // iterator points to a Node*
    REQUIRE((*it)->getId() == 0);   // node 0 should be closest neighbor

    it = V_set.begin();
    REQUIRE((*it)->getId() == 0);
}