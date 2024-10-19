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
