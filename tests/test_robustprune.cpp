#include "catch.hpp"
#include "../include/graph.hpp"
#include "../include/utility.hpp"
#include "../include/robustprune.hpp"

TEST_CASE("RobustPrune function test", "[RobustPruneBasic]") {
    Graph G;
    Node* p = new Node(1, {0.0, 0.0}, {}, {});  // Node `p` at origin
    Node* n1 = new Node(2, {1.0, 1.0}, {}, {});
    Node* n2 = new Node(3, {2.0, 2.0}, {}, {});
    Node* n3 = new Node(4, {3.0, 3.0}, {}, {});
    Node* n4 = new Node(5, {4.0, 4.0}, {}, {});

    G.addNode(p);
    G.addNode(n1);
    G.addNode(n2);
    G.addNode(n3);
    G.addNode(n4);

    // Initialize candidate set V with some nodes
    set<Node*> V = {n1, n2, n3, n4};

    // Define parameters
    double a = 1.5;  // Distance threshold
    int R = 2;       // Degree bound (max out-neighbors for p)

    // Run RobustPrune on node `p`
    RobustPrune(p, V, a, R);

    // Check that the number of out-neighbors is at most R
    REQUIRE(p->getEdges().size() <= static_cast<size_t>(R));

    // Convert p->getEdges() to a set for faster lookup.
    set<int> neighborsId;

    for (Node* neighbor : p->getEdges()) {
        neighborsId.insert(neighbor->getId());
    }

    Node* pStar = NULL;
    auto neighbors = p->getEdges();
    if (!neighbors.empty()) {
        pStar = neighbors.back();  // Approximate pStar as the last-added neighbor
    }

    // Additional checks:
    // 1. Each selected out-neighbor should meet the distance threshold
    for (Node* neighbor : p->getEdges()) {
        for (Node* v : V) {
            // Check if `v` is not in `p`'s edges by looking it up in `neighborsId`
            if (neighborsId.find(v->getId()) == neighborsId.end()) {
                // Neighbor `v` should have been removed if it didn't meet the threshold
                REQUIRE(a * euclidean_distance_of_nodes(p, neighbor) > euclidean_distance_of_nodes(pStar, v));
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

TEST_CASE("RobustPrune with empty candidate set V", "[RobustPruneEmptyV]") {
    Graph G;
    Node* p = new Node(1, {0.0, 0.0}, {}, {});
    G.addNode(p);

    set<Node*> V = {};  // Empty candidate set
    double a = 1.5;
    int R = 3;

    RobustPrune(p, V, a, R);

    REQUIRE(p->getEdges().empty());  // No neighbors should be added

    delete p;
}

TEST_CASE("RobustPrune with V containing only p itself", "[RobustPruneVOnlyp]") {
    Graph G;
    Node* p = new Node(1, {0.0, 0.0}, {}, {});
    G.addNode(p);

    set<Node*> V = {p};  // `V` contains only `p` itself
    double a = 1.5;
    int R = 3;

    RobustPrune(p, V, a, R);

    REQUIRE(p->getEdges().empty());  // `p` should not add itself as a neighbor

    delete p;
}

TEST_CASE("RobustPrune with R larger than V size", "[RobustPruneRLargerThanV]") {
    Graph G;
    Node* p = new Node(1, {0.0, 0.0}, {}, {});
    Node* n1 = new Node(2, {1.0, 1.0}, {}, {});
    Node* n2 = new Node(3, {2.0, 2.0}, {}, {});
    G.addNode(p);
    G.addNode(n1);
    G.addNode(n2);

    set<Node*> V = {n1, n2};
    double a = 2.0;
    int R = 5;  // Larger than `V` size

    RobustPrune(p, V, a, R);

    REQUIRE(p->getEdges().size() <= V.size());
    REQUIRE((p->edgeExists(n1->getId()) || p->edgeExists(n2->getId())));

    delete p;
    delete n1;
    delete n2;
}

TEST_CASE("RobustPrune with a tight distance threshold (a = 1)", "[RobustPruneaEquals1]") {
    Graph G;
    Node* p = new Node(1, {0.0, 0.0}, {}, {});
    Node* n1 = new Node(2, {1.0, 1.0}, {}, {});  // Close to p
    Node* n2 = new Node(3, {5.0, 5.0}, {}, {});  // Farther from p
    G.addNode(p);
    G.addNode(n1);
    G.addNode(n2);

    set<Node*> V = {n1, n2};
    double a = 1.0;  // Tight distance threshold
    int R = 2;

    RobustPrune(p, V, a, R);

    REQUIRE(p->edgeExists(n1->getId()));     // `n1` should be added
    REQUIRE_FALSE(p->edgeExists(n2->getId()));  // `n2` should be excluded due to distance

    delete p;
    delete n1;
    delete n2;
}

TEST_CASE("RobustPrune with high R and large candidate set V", "[RobustPruneLargeRandV]") {
    Graph G;
    Node* p = new Node(1, {0.0, 0.0}, {}, {});
    G.addNode(p);

    // Add 10 nodes around `p`
    set<Node*> V;
    for (int i = 2; i <= 11; i++) {
        Node* ni = new Node(i, {static_cast<double>(i), static_cast<double>(i)}, {}, {});
        G.addNode(ni);
        V.insert(ni);
    }

    double a = 4.0;
    int R = 5;  // Request up to 5 neighbors

    RobustPrune(p, V, a, R);

    REQUIRE(p->getEdges().size() == static_cast<size_t>(R));  // Should have exactly `R` out-neighbors

    for (Node* ni : V) {
        delete ni;
    }
    delete p;
}
