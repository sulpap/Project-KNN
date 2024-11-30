#include "catch.hpp"
#include "../include/filteredGreedySearch.hpp"

TEST_CASE("FilteredGreedySearch in fully disconnected graph - filtered vamana call simulator first iter of loop") {
// Ο γράφος μας περιέχει μόνο 2 nodes με διαφορετικά filters το καθένα + not connected
// We call greedy for a point that matches one of the two filters
    Graph graph;            

    // Graph must contain starting node
    Node start_node = Node(0, {2.0, 3.0}, {}, 2);       // filter = 2
    graph.addNode(&start_node);

    // Graph will probably have another point from the dataset
    Node another_node = Node(1, {4.0, 4.0}, {}, 1);      // filter = 1
    graph.addNode(&another_node);

    // We assume that pointCoords is a point from the dataset and not a query
    vector<double> pointCoords = {5.0, 6.0};

    set<Node*> S_set;
    S_set.insert(&start_node);

    set<Node*> L_set;
    set<Node*> V_set;

    set<int> F_q_set;
    F_q_set.insert(2);      // filter of point is 2


    int k = 0;
    int L = 2;

    FilteredGreedySearch(S_set, pointCoords, k, L, L_set, V_set, F_q_set);

    REQUIRE(L_set.size() == 0);
    auto it = V_set.begin();        // iterator points to a Node*
    REQUIRE(*it == &start_node);   
}

TEST_CASE("FilteredGreedySearch for query with filter and no filter - main call simulator, index constructed by filtered vamana") {
// Ο γράφος μας αποτελείται από 2 συνεκτικές συνιστώσες
// Η 1η συνιστώσα αφορά το φίλτρο 2, ενώ η 2η συνιστώσα αφορά το φίλτρο 5
// Το query έχει φίλτρο 2
    Graph graph;

    Node s = Node(0,{3.0, 3.0}, {}, 2);     // start node of filter 2
    Node q = Node(1, {1.0, 1.0}, {}, 2);    // a node with filter 2
    Node p = Node(2, {2.0, 3.0}, {}, 2);    // another node with filter 2    
    graph.addNode(&s);
    graph.addNode(&q);
    graph.addNode(&p);

    graph.addEdge(0, 2);            // s -> p
    graph.addEdge(2, 0);            // p -> s
    graph.addEdge(1, 2);            // q -> p

    Node l = Node(3, {5.0, 5.0}, {}, 5);    // start node of filter 5
    Node r = Node(4, {4.0, 4.0}, {}, 5);    // a node with filter 5
    graph.addNode(&l);
    graph.addNode(&r);

    graph.addEdge(3, 4);            // l -> r
    graph.addEdge(4, 3);            // r -> l

    vector<double> queryCoords = {0.0, 0.0};

// [1] Κλήση σε query με ίδιο φίλτρο για τη 1η συνιστώσα του γράφου
    set<Node*> S_set;
    S_set.insert(&s);

    set<Node*> L_set;
    set<Node*> V_set;

    set<int> F_q_set;
    F_q_set.insert(2);      // filter of query is 2

    int k = 2;
    int L = 4;

    FilteredGreedySearch(S_set, queryCoords, k, L, L_set, V_set, F_q_set);

    REQUIRE(L_set.size() == 2);
    REQUIRE(V_set.size() == 2);
    REQUIRE(L_set == V_set);            // They should both have s, p

    set<Node*> set_test;
    set_test.insert(&s);
    set_test.insert(&p);
    REQUIRE(L_set == set_test);

// [2] Κλήση σε query με ίδιο φίλτρο για τη 2η συνιστώσα του γράφου
    L_set.clear();                  // L_set, V_set should be empty before new call of greedy
    V_set.clear();
    S_set.clear();
    F_q_set.clear();
    set_test.clear();

    S_set.insert(&l);
    k = 1;
    L = 4;
    F_q_set.insert(5);              // filter of query is 5

    FilteredGreedySearch(S_set, queryCoords, k, L, L_set, V_set, F_q_set);

    REQUIRE(L_set.size() == 1);
    REQUIRE(V_set.size() == 2);
    auto it = L_set.begin();        // iterator points to a Node*
    REQUIRE(*it == &r);

    set_test.insert(&l);
    set_test.insert(&r);
    REQUIRE(V_set == set_test);

// [3] Κλήση σε unfiltered query
    L_set.clear();                  // L_set, V_set should be empty before new call of greedy
    V_set.clear();
    S_set.clear();
    F_q_set.clear();
    set_test.clear();

    S_set.insert(&s);           // starting node for filter = 2
    S_set.insert(&l);           // starting node for filter = 5
    k = 2;
    L = 4;
    // Unfiltered search, therefore query has all filters
    F_q_set.insert(2);          
    F_q_set.insert(5);  

    FilteredGreedySearch(S_set, queryCoords, k, L, L_set, V_set, F_q_set);

    REQUIRE(L_set.size() == 2);
    REQUIRE(V_set.size() == 4);

    set_test.insert(&s);
    set_test.insert(&p);
    REQUIRE(L_set == set_test);

    set_test.insert(&l);
    set_test.insert(&r);
    REQUIRE(V_set == set_test);
}