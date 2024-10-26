#include "catch.hpp"
#include "../include/graph.hpp"
#include "../include/utility.hpp"
#include "../include/robustprune.hpp"

TEST_CASE("RobustPrune function test") {
    // Set up nodes with sample coordinates in a graph
    Graph G;
    Node* p = new Node(1, {0.0, 0.0}, {});  // Node `p` at origin
    Node* n1 = new Node(2, {1.0, 1.0}, {});
    Node* n2 = new Node(3, {2.0, 2.0}, {});
    Node* n3 = new Node(4, {3.0, 3.0}, {});
    Node* n4 = new Node(5, {4.0, 4.0}, {});

    // Add nodes to graph G
    G.addNode(p);
    G.addNode(n1);
    G.addNode(n2);
    G.addNode(n3);
    G.addNode(n4);

    // Initialize candidate set V with some nodes
    std::set<Node*> V = {n1, n2, n3, n4};

    // Define parameters
    double a = 1.5;  // Distance threshold
    int R = 2;       // Degree bound (max out-neighbors for p)

    // Run RobustPrune on node `p`
    RobustPrune(G, p, V, a, R);

    // Check that the number of out-neighbors is at most R
    REQUIRE(p->getEdges().size() <= static_cast<size_t>(R));

    // Additional checks:
    // 1. Each selected out-neighbor should meet the distance threshold
    for (Node* neighbor : p->getEdges()) {
        for (Node* v : V) {
            if (std::find(p->getEdges().begin(), p->getEdges().end(), v) == p->getEdges().end()) {
                // Neighbor `v` should have been removed if it didn't meet the threshold
                REQUIRE(a * euclidean_distance_of_nodes(p, neighbor) > euclidean_distance_of_nodes(p, v));
            }
        }
    }

    // Clean up
    delete p;
    delete n1;
    delete n2;
    delete n3;
    delete n4;
}
