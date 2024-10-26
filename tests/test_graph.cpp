#include "catch.hpp"
#include "../include/graph.hpp"

TEST_CASE("Node Class Tests", "[Node]") {
    // Test Node creation
    std::vector<double> coordinates = {1.0, 2.0};
    Node node(1, coordinates, {});

    SECTION("Node Initialization") {
        REQUIRE(node.getId() == 1);
        REQUIRE(node.getGraphId() == 0);
        REQUIRE(node.getCoordinates() == coordinates);
        REQUIRE(node.getEdges().empty());
    }

    SECTION("Setters and Getters") {
        node.setId(2);
        node.setGraphId(3);
        std::vector<double> newCoordinates = {4.0, 5.0};
        node.setCoordinates(newCoordinates);

        REQUIRE(node.getId() == 2);
        REQUIRE(node.getGraphId() == 3);
        REQUIRE(node.getCoordinates() == newCoordinates);
    }

    SECTION("Add Edge and Check Existence") {
        Node otherNode(2, {3.0, 4.0}, {});
        node.addEdge(&otherNode);
        REQUIRE(node.getEdges().size() == 1);
        REQUIRE(node.edgeExists(2) == true);
        REQUIRE(node.edgeExists(3) == false);
    }

    SECTION("Delete Edge") {
        Node otherNode(2, {3.0, 4.0}, {});
        node.addEdge(&otherNode);
        node.deleteEdge(2);
        REQUIRE(node.getEdges().empty());
    }

    SECTION("Access Specific Coordinate") {
        REQUIRE(node.getSpecificCoordinate(0) == 1.0);
        REQUIRE(node.getSpecificCoordinate(1) == 2.0);
        REQUIRE_THROWS_AS(node.getSpecificCoordinate(2), std::out_of_range);
    }

    SECTION("Set Specific Coordinate") {
        node.setSpecificCoordinate(0, 10.0);
        REQUIRE(node.getSpecificCoordinate(0) == 10.0);
    }
}

TEST_CASE("Graph Class Tests", "[Graph]") {
    Graph graph;

    SECTION("Graph Initialization") {
        REQUIRE(graph.getGraphId() == 1); // Check initial graph ID
        REQUIRE(graph.getAdjList().empty()); // Ensure adjacency list is empty
    }

    SECTION("Add Node") {
        Node* node = new Node(1, {1.0, 2.0}, {});
        graph.addNode(node);
        REQUIRE(graph.getAdjList().size() == 1);
        REQUIRE(graph.getNode(1) == node);
        REQUIRE(node->getGraphId() == graph.getGraphId());
    }

    SECTION("Delete Node") {
        Node* node = new Node(1, {1.0, 2.0}, {});
        graph.addNode(node);
        graph.deleteNode(1);
        REQUIRE(graph.getAdjList().size() == 0);
        REQUIRE(graph.getNode(1) == nullptr);
    }

    SECTION("Add Edge") {
        Node* node1 = new Node(1, {1.0, 2.0}, {});
        Node* node2 = new Node(2, {3.0, 4.0}, {});
        graph.addNode(node1);
        graph.addNode(node2);
        graph.addEdge(1, node2);
        REQUIRE(node1->getEdges().size() == 1);
        REQUIRE(node1->edgeExists(2) == true);
        REQUIRE(node1->getEdges().front() == node2);
    }

    SECTION("Remove Edge") {
        Node* node1 = new Node(1, {1.0, 2.0}, {});
        Node* node2 = new Node(2, {3.0, 4.0}, {});
        graph.addNode(node1);
        graph.addNode(node2);
        graph.addEdge(1, node2);
        graph.removeEdge(1, 2);
        REQUIRE(node1->getEdges().empty());
    }

    SECTION("Get Node") {
        Node* node = new Node(1, {1.0, 2.0}, {});
        graph.addNode(node);
        REQUIRE(graph.getNode(1) == node);
        REQUIRE(graph.getNode(2) == nullptr);
    }

    SECTION("Union of Graphs") {
        Graph otherGraph;
        Node* node1 = new Node(1, {1.0, 2.0}, {});
        Node* node2 = new Node(2, {3.0, 4.0}, {});
        otherGraph.addNode(node1);
        otherGraph.addNode(node2);

        CHECK(otherGraph.getNode(1)->getId() == 1);
        CHECK(otherGraph.getNode(1)->getGraphId() == otherGraph.getGraphId());
        CHECK(otherGraph.getNode(1)->getCoordinates().at(0) == 1.0);
        CHECK(otherGraph.getNode(1)->getCoordinates().at(1) == 2.0);
        CHECK(otherGraph.getNode(1)->getEdges().empty() == true);

        CHECK(otherGraph.getNode(2)->getId() == 2);
        CHECK(otherGraph.getNode(2)->getGraphId() == otherGraph.getGraphId());
        CHECK(otherGraph.getNode(2)->getCoordinates().at(0) == 3.0);
        CHECK(otherGraph.getNode(2)->getCoordinates().at(1) == 4.0);
        CHECK(otherGraph.getNode(2)->getEdges().empty() == true);

        graph.graphUnion(otherGraph);
        REQUIRE(graph.getAdjList().size() == 2);

        REQUIRE(graph.getNode(1)->getId() == 1);
        REQUIRE(graph.getNode(1)->getGraphId() == graph.getGraphId());
        REQUIRE(graph.getNode(1)->getCoordinates().at(0) == 1.0);
        REQUIRE(graph.getNode(1)->getCoordinates().at(1) == 2.0);
        REQUIRE(graph.getNode(1)->getEdges().empty() == true);

        REQUIRE(graph.getNode(2)->getId() == 2);
        REQUIRE(graph.getNode(2)->getGraphId() == graph.getGraphId());
        REQUIRE(graph.getNode(2)->getCoordinates().at(0) == 3.0);
        REQUIRE(graph.getNode(2)->getCoordinates().at(1) == 4.0);
        REQUIRE(graph.getNode(2)->getEdges().empty() == true);
    }

    // SECTION("Union of Graphs with same Nodes") {
    //     Graph otherGraph;
    //     Node* node1_1 = new Node(1, {1.0, 2.0}, {});
    //     Node* node2_1 = new Node(2, {3.0, 4.0}, {});

    //     Node* node1_2 = new Node(1, {1.0, 2.0}, {});
    //     Node* node2_2 = new Node(2, {3.0, 4.0}, {});

    //     graph.addNode(node1_1);
    //     graph.addNode(node2_1);
    //     otherGraph.addNode(node1_2);
    //     otherGraph.addNode(node2_2);
    // }

    // Clean up dynamically allocated nodes
    for (auto& pair : graph.getAdjList()) {
        delete pair.second;
    }
}


// Experimenting

TEST_CASE("Graph Union Test - Basic") {
    // Create graph1 with some nodes and edges
    Graph graph1;
    Node* node1 = new Node(1, {0.0, 1.0}, {});
    Node* node2 = new Node(2, {1.0, 1.0}, {});
    graph1.addNode(node1);
    graph1.addNode(node2);
    graph1.addEdge(1, 2);

    // Create graph2 with a new node and an overlapping node
    Graph graph2;
    Node* node2_copy = new Node(2, {1.0, 1.0}, {}); // Same ID as in graph1
    Node* node3 = new Node(3, {2.0, 2.0}, {});
    graph2.addNode(node2_copy);
    graph2.addNode(node3);
    graph2.addEdge(2, 3);

    // Perform the union
    graph1.graphUnion(graph2);

    SECTION("Union adds new nodes correctly") {
        REQUIRE(graph1.getNode(3) != nullptr);  // Node 3 should now exist
    }

    SECTION("Union doesn't duplicate existing nodes") {
        REQUIRE(graph1.getNode(2)->getCoordinates() == std::vector<double>({1.0, 1.0}));
    }

    SECTION("Union adds edges correctly") {
        Node* node2_after_union = graph1.getNode(2);
        REQUIRE(node2_after_union->edgeExists(3)); // Edge 2 -> 3 should be added
    }
}

TEST_CASE("Graph Union Test - Empty Graph") {
    // Create two graphs, one empty and one with nodes
    Graph graph1;
    Node* node1 = new Node(1, {0.0, 1.0}, {});
    graph1.addNode(node1);

    Graph emptyGraph;

    // Perform union with an empty graph
    graph1.graphUnion(emptyGraph);

    SECTION("Union with empty graph doesn't alter the original graph") {
        REQUIRE(graph1.getNode(1) != nullptr); // Node 1 should still exist
        REQUIRE(graph1.getNode(2) == nullptr); // No new nodes should be added
    }
}

TEST_CASE("Graph Union Test - Self-loops Handling") {
    Graph graph1;
    Node* node1 = new Node(1, {0.0, 1.0}, {});
    node1->addEdge(node1); // Self-loop
    graph1.addNode(node1);

    Graph graph2;
    Node* node2 = new Node(2, {1.0, 1.0}, {});
    graph2.addNode(node2);

    // Perform the union
    graph1.graphUnion(graph2);

    SECTION("Union doesn't duplicate self-loops") {
        Node* node1_after_union = graph1.getNode(1);
        REQUIRE(node1_after_union->edgeExists(1)); // Ensure self-loop exists
    }
}


TEST_CASE("Graph Intersection Test - Basic") {
    // Create graph1 with some nodes and edges
    Graph graph1;
    Node* node1 = new Node(1, {0.0, 1.0}, {});
    Node* node2 = new Node(2, {1.0, 1.0}, {});
    graph1.addNode(node1);
    graph1.addNode(node2);
    graph1.addEdge(1, 2);

    // Create graph2 with an overlapping node and an edge
    Graph graph2;
    Node* node2_copy = new Node(2, {1.0, 1.0}, {});
    graph2.addNode(node2_copy);
    Node* node3 = new Node(3, {2.0, 2.0}, {});
    graph2.addNode(node3);
    graph2.addEdge(2, 3);

    // Perform the intersection
    graph1.graphIntersection(graph2);

    SECTION("Intersection retains common nodes") {
        REQUIRE(graph1.getNode(2) != nullptr);  // Node 2 should still exist
    }

    SECTION("Intersection removes non-common nodes") {
        REQUIRE(graph1.getNode(1) == nullptr);  // Node 1 should be removed
        REQUIRE(graph1.getNode(3) == nullptr);  // Node 3 shouldn't exist in graph1
    }

    SECTION("Intersection retains common edges") {
        REQUIRE(graph1.getNode(2)->getEdges().empty());  // No common edges should remain
    }
}

TEST_CASE("Graph Intersection Test - Empty Intersection") {
    // Create two graphs with no overlapping nodes or edges
    Graph graph1;
    Node* node1 = new Node(1, {0.0, 1.0}, {});
    graph1.addNode(node1);

    Graph graph2;
    Node* node2 = new Node(2, {1.0, 1.0}, {});
    graph2.addNode(node2);

    // Perform intersection
    graph1.graphIntersection(graph2);

    SECTION("Intersection of disjoint graphs results in empty graph") {
        REQUIRE(graph1.getAdjList().empty()); // graph1 should now be empty
    }
}

TEST_CASE("Graph Intersection Test - Self-loops Handling") {
    Graph graph1;
    Node* node1 = new Node(1, {0.0, 1.0}, {});
    node1->addEdge(node1); // Self-loop
    graph1.addNode(node1);

    Graph graph2;
    Node* node1_copy = new Node(1, {0.0, 1.0}, {});
    node1_copy->addEdge(node1_copy);
    graph2.addNode(node1_copy);

    // Perform the intersection
    graph1.graphIntersection(graph2);

    SECTION("Intersection retains self-loops if they are present in both graphs") {
        REQUIRE(graph1.getNode(1) != nullptr);     // Node 1 should still exist
        REQUIRE(graph1.getNode(1)->edgeExists(1)); // Self-loop should be retained
    }
}


