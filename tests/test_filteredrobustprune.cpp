#include "catch.hpp"
#include "../include/graph.hpp"
#include "../include/utility.hpp"
#include "../include/filteredrobustprune.hpp"

// TEST_CASE("FilteredRobustPrune basic test with labels", "[FilteredRobustPruneBasic]") {
//     Graph G;
//     Node* p = new Node(1, {0.0, 0.0}, {}, 1);  // Node `p` with label 1
//     Node* n1 = new Node(2, {1.0, 1.0}, {}, 1); // Compatible label
//     Node* n2 = new Node(3, {2.0, 2.0}, {}, 2); // Incompatible label
//     Node* n3 = new Node(4, {3.0, 3.0}, {}, 1); // Compatible label
//     Node* n4 = new Node(5, {4.0, 4.0}, {}, 2); // Incompatible label

//     G.addNode(p);
//     G.addNode(n1);
//     G.addNode(n2);
//     G.addNode(n3);
//     G.addNode(n4);

//     set<Node*> V = {n1, n2, n3, n4};
//     double a = 2.0; // Distance threshold multiplier
//     int R = 2;      // Max out-neighbors

//     FilteredRobustPrune(p, V, a, R);

//     REQUIRE(p->getEdges().size() == static_cast<size_t>(R));
//     for (Node* neighbor : p->getEdges()) {
//         REQUIRE(neighbor->getLabel() == 1); // Check label compatibility
//     }

//     // Convert p->getEdges() to a set for faster lookup.
//     set<int> neighborsId;

//     for (Node* neighbor : p->getEdges()) {
//         neighborsId.insert(neighbor->getId());
//     }

//     Node* pStar = NULL;
//     auto neighbors = p->getEdges();
//     if (!neighbors.empty()) {
//         pStar = neighbors.back();  // Approximate pStar as the last-added neighbor
//     }

//     // Additional checks:
//     // 1. Each selected out-neighbor should meet the distance threshold
//     for (Node* neighbor : p->getEdges()) {
//         for (Node* v : V) {
//             // Check if `v` is not in `p`'s edges by looking it up in `neighborsId`
//             if (neighborsId.find(v->getId()) == neighborsId.end()) {
//                 // Neighbor `v` should have been removed if it didn't meet the threshold
//                 REQUIRE(a * euclidean_distance_of_nodes(p, neighbor) > euclidean_distance_of_nodes(pStar, v));
//             }
//         }
//     }

//     delete p;
//     delete n1;
//     delete n2;
//     delete n3;
//     delete n4;
// }

TEST_CASE("FilteredRobustPrune with empty candidate set V", "[FilteredRobustPruneEmptyV]") {
    Graph G;
    Node* p = new Node(1, {0.0, 0.0}, {}, 1); // Node `p` with label 1
    G.addNode(p);

    set<Node*> V = {}; // Empty candidate set
    double a = 1.5;
    int R = 3;

    FilteredRobustPrune(p, V, a, R);

    REQUIRE(p->getEdges().empty()); // No neighbors should be added

    delete p;
}

TEST_CASE("FilteredRobustPrune with V containing only p itself", "[FilteredRobustPruneVOnlyp]") {
    Graph G;
    Node* p = new Node(1, {0.0, 0.0}, {}, 1);
    G.addNode(p);

    set<Node*> V = {p};  // `V` contains only `p` itself
    double a = 1.5;
    int R = 3;

    FilteredRobustPrune(p, V, a, R);

    REQUIRE(p->getEdges().empty());  // `p` should not add itself as a neighbor

    delete p;
}

TEST_CASE("FilteredRobustPrune with R larger than V size", "[FilteredRobustPruneRLargerThanV]") {
    Graph G;
    Node* p = new Node(1, {0.0, 0.0}, {}, 1);  // Node `p` with label 1
    Node* n1 = new Node(2, {1.0, 1.0}, {}, 1); // Compatible label
    Node* n2 = new Node(3, {2.0, 2.0}, {}, 1); // Compatible label
    G.addNode(p);
    G.addNode(n1);
    G.addNode(n2);

    set<Node*> V = {n1, n2};
    double a = 2.0;
    int R = 5; // Larger than `V` size

    FilteredRobustPrune(p, V, a, R);

    REQUIRE(p->getEdges().size() <= V.size());
    REQUIRE((p->edgeExists(n1->getId()) || p->edgeExists(n2->getId())));
    for (Node* neighbor : p->getEdges()) {
        REQUIRE(neighbor->getLabel() == 1); // Label compatibility
    }

    delete p;
    delete n1;
    delete n2;
}

// TEST_CASE("FilteredRobustPrune with incompatible labels in V", "[FilteredRobustPruneIncompatibleLabels]") {
//     Graph G;
//     Node* p = new Node(1, {0.0, 0.0}, {}, 1);  // Node `p` with label 1
//     Node* n1 = new Node(2, {1.0, 1.0}, {}, 2); // Incompatible label
//     Node* n2 = new Node(3, {2.0, 2.0}, {}, 3); // Incompatible label
//     G.addNode(p);
//     G.addNode(n1);
//     G.addNode(n2);

//     set<Node*> V = {n1, n2};
//     double a = 1.5;
//     int R = 3;

//     FilteredRobustPrune(p, V, a, R);

//     REQUIRE(p->getEdges().empty()); // No neighbors due to incompatible labels

//     delete p;
//     delete n1;
//     delete n2;
// }

TEST_CASE("FilteredRobustPrune with a tight distance threshold", "[FilteredRobustPruneTightThreshold]") {
    Graph G;
    Node* p = new Node(1, {0.0, 0.0}, {}, 1);  // Node `p` with label 1
    Node* n1 = new Node(2, {1.0, 1.0}, {}, 1); // Compatible label, close
    Node* n2 = new Node(3, {5.0, 5.0}, {}, 1); // Compatible label, far
    G.addNode(p);
    G.addNode(n1);
    G.addNode(n2);

    set<Node*> V = {n1, n2};
    double a = 1.0; // Tight distance threshold
    int R = 2;

    FilteredRobustPrune(p, V, a, R);

    REQUIRE(p->edgeExists(n1->getId()));       // `n1` should be added
    REQUIRE_FALSE(p->edgeExists(n2->getId())); // `n2` should be excluded

    delete p;
    delete n1;
    delete n2;
}

// TEST_CASE("FilteredRobustPrune with label compatibility checks", "[FilteredRobustPruneLabelChecks]") {
//     Graph G;
//     Node* p = new Node(1, {0.0, 0.0}, {}, 1); // Node `p` with label 1
//     Node* n1 = new Node(2, {1.0, 1.0}, {}, 2); // Incompatible
//     Node* n2 = new Node(3, {2.0, 2.0}, {}, -1); // Neutral label
//     Node* n3 = new Node(4, {3.0, 3.0}, {}, 1); // Compatible

//     G.addNode(p);
//     G.addNode(n1);
//     G.addNode(n2);
//     G.addNode(n3);

//     set<Node*> V = {n1, n2, n3};
//     double a = 1.5;
//     int R = 2;

//     FilteredRobustPrune(p, V, a, R);

//     REQUIRE(p->getEdges().size() == 2); // Should add up to 2 neighbors
//     REQUIRE(p->edgeExists(n3->getId())); // Compatible neighbor added
//     REQUIRE(p->edgeExists(n2->getId())); // Neutral neighbor added

//     delete p;
//     delete n1;
//     delete n2;
//     delete n3;
// }

TEST_CASE("FilteredRobustPrune with high R and large candidate set V", "[FilteredRobustPruneLargeRandV]") {
    Graph G;
    Node* p = new Node(1, {0.0, 0.0}, {}, 1);
    G.addNode(p);

    // Add 10 nodes around `p`
    set<Node*> V;
    for (int i = 2; i <= 11; i++) {
        Node* ni = new Node(i, {static_cast<double>(i), static_cast<double>(i)}, {}, 1);
        G.addNode(ni);
        V.insert(ni);
    }

    double a = 4.0;
    int R = 5;  // Request up to 5 neighbors

    FilteredRobustPrune(p, V, a, R);

    REQUIRE(p->getEdges().size() == static_cast<size_t>(R));  // Should have exactly `R` out-neighbors

    for (Node* ni : V) {
        delete ni;
    }
    delete p;
}