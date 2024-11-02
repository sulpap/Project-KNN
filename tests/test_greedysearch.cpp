#include "catch.hpp"
#include "../include/graph.hpp"
#include "../include/greedysearch.hpp"

TEST_CASE("GreedySearch in small connected graph") {
    Graph graph;

    // list initialization syntax available for C++11 and newer
    // {1.0, 1.0} initializes the std::vector<double> with two elements.
    // {} creates an empty std::list<Node*>
    Node n0 = Node(0, {1.0, 1.0}, {});
    Node n1 = Node(1, {1.0, 3.0}, {});
    Node n2 = Node(2, {2.0, 2.0}, {});

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

    it = L_set.begin();        // iterator points to a Node*
    REQUIRE((*it)->getId() == 2);   // node 2 should be closest neighbor
    
    REQUIRE(V_set == set_test);

// 4th run k = 1, L = 5 (L > number of nodes in graph):
    L_set.clear();
    V_set.clear();
    GreedySearch(&n0, query, 1, 5, L_set, V_set);
    REQUIRE(L_set.size() == 1);

    it = L_set.begin();        // iterator points to a Node*
    REQUIRE((*it)->getId() == 2);   // node 2 should be closest neighbor
    
    REQUIRE(V_set == set_test);
}

TEST_CASE("GreedySearch in fully disconnected small graph") {
    Graph graph;

    Node n0 = Node(0, {1.0, 1.0}, {});
    Node n1 = Node(1, {1.0, 3.0}, {});
    Node n2 = Node(2, {2.0, 2.0}, {});

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

    it = L_set.begin();        // iterator points to a Node*
    REQUIRE((*it)->getId() == 0);   // node 0 should be closest neighbor

    it = V_set.begin();
    REQUIRE((*it)->getId() == 0);
}