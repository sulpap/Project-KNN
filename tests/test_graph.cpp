#include "catch.hpp"
#include "../include/graph.hpp"

TEST_CASE("Node Class Tests", "[Node]")
{
    // Test Node creation
    std::vector<double> coordinates = {1.0, 2.0};
    Node node(1, coordinates, {}, 1);

    SECTION("Node Initialization")
    {
        REQUIRE(node.getId() == 1);
        REQUIRE(node.getGraphId() == 0);
        REQUIRE(node.getCoordinates() == coordinates);
        REQUIRE(node.getEdges().empty());
        REQUIRE(node.getLabel() == 1);
    }

    SECTION("Move Constructor")
    {
        Node* node1 = new Node(1, {1.0, 2.0}, {});
        Node* node2 = new Node(2, {3.0, 4.0}, {});
        node1->addEdge(node2);

        // Move construct a new node
        Node* node3 = new Node(move(*node1)); // Move by value

        // Check node3 has the data
        REQUIRE(node3->getId() == 1);
        REQUIRE(node3->getCoordinates() == vector<double>{1.0, 2.0});
        REQUIRE(!node3->getEdges().empty());

        // Check node1 is invalidated
        REQUIRE(node1->getLabel() == -1);
        REQUIRE(node1->getEdges().empty());

        delete node1; // Now safe to delete invalidated node
        delete node2;
        delete node3;
    }


    SECTION("Setters and Getters")
    {
        node.setId(2);
        node.setGraphId(3);
        std::vector<double> newCoordinates = {4.0, 5.0};
        node.setCoordinates(newCoordinates);
        node.setLabel(1);

        REQUIRE(node.getId() == 2);
        REQUIRE(node.getGraphId() == 3);
        REQUIRE(node.getCoordinates() == newCoordinates);
        REQUIRE(node.getLabel() == 1);
    }

    SECTION("Move Assignment Operator")
    {
        Node* node1 = new Node(1, {1.0, 2.0}, {});
        Node* node2 = new Node(2, {3.0, 4.0}, {});
        node1->addEdge(node2);

        // Create another node and move-assign
        Node* node3 = new Node(3, {0.0, 0.0}, {});
        *node3 = move(*node1); // Move by assignment

        // Check node3 has the data
        REQUIRE(node3->getId() == 1);
        REQUIRE(node3->getCoordinates() == vector<double>{1.0, 2.0});
        REQUIRE(!node3->getEdges().empty());

        // Check node1 is invalidated
        REQUIRE(node1->getLabel() == -1);
        REQUIRE(node1->getEdges().empty());

        delete node1; // Now safe to delete invalidated node
        delete node2;
        delete node3;
    }

    SECTION("Add Edge and Check Existence")
    {
        Node otherNode(2, {3.0, 4.0}, {}, {});
        node.addEdge(&otherNode);
        REQUIRE(node.getEdges().size() == 1);
        REQUIRE(node.edgeExists(2) == true);
        REQUIRE(node.edgeExists(3) == false);
    }

    SECTION("Delete Edge")
    {
        Node otherNode(2, {3.0, 4.0}, {});
        node.addEdge(&otherNode);
        node.deleteEdge(2);
        REQUIRE(node.getEdges().empty());
    }

    SECTION("Access Specific Coordinate")
    {
        REQUIRE(node.getSpecificCoordinate(0) == 1.0);
        REQUIRE(node.getSpecificCoordinate(1) == 2.0);
        REQUIRE_THROWS_AS(node.getSpecificCoordinate(2), std::out_of_range);
    }

    SECTION("Set Specific Coordinate")
    {
        node.setSpecificCoordinate(0, 10.0);
        REQUIRE(node.getSpecificCoordinate(0) == 10.0);
        REQUIRE_THROWS_AS(node.setSpecificCoordinate(2, 1.0), std::out_of_range);
    }
}

TEST_CASE("Graph Class Tests", "[Graph]")
{
    int temp_currentGraphId = Graph::getCurrentGraphId();
    Graph graph;

    SECTION("Graph Initialization")
    {
        REQUIRE(graph.getGraphId() == temp_currentGraphId); // Check initial graph ID
        REQUIRE(graph.getAdjList().empty());                // Ensure adjacency list is empty
    }

    SECTION("Clear Graph")
    {
        Node *node1 = new Node(1, {1.0, 2.0}, {});
        Node *node2 = new Node(2, {3.0, 4.0}, {});
        graph.addNode(node1);
        graph.addNode(node2);

        REQUIRE(graph.getAdjList().size() == 2); // Ensure nodes are added
        graph.clear();                           // Clear the graph

        REQUIRE(graph.getAdjList().empty()); // Ensure adjacency list is empty
    }

    SECTION("Add Node")
    {
        Node *node = new Node(1, {1.0, 2.0}, {});
        graph.addNode(node);
        REQUIRE(graph.getAdjList().size() == 1);
        REQUIRE(graph.getNode(1) == node);
        int gid = node->getGraphId();
        REQUIRE(gid == graph.getGraphId());
    }

    SECTION("Move Node")
    {
        Graph graph;

        // Create a node
        Node* node = new Node(1, {1.0, 1.0}, {});

        // Move node into the graph
        graph.moveNode(node);

        // Verify the node is in the adjacency list
        REQUIRE(graph.getNode(1) == node);

        // Verify the node's graph ID matches the graph's ID
        REQUIRE(node->getGraphId() == graph.getGraphId());
        
        graph.clear();
    }

    SECTION("Delete Node")
    {
        Node *node = new Node(1, {1.0, 2.0}, {});
        graph.addNode(node);
        graph.deleteNode(1);
        REQUIRE(graph.getAdjList().size() == 0);
        REQUIRE(graph.getNode(1) == nullptr);
    }

    SECTION("Add Edge")
    {
        Node *node1 = new Node(1, {1.0, 2.0}, {});
        Node *node2 = new Node(2, {3.0, 4.0}, {});
        graph.addNode(node1);
        graph.addNode(node2);
        graph.addEdge(1, node2);
        REQUIRE(node1->getEdges().size() == 1);
        REQUIRE(node1->edgeExists(2) == true);
        REQUIRE(node1->getEdges().front() == node2);
    }

    SECTION("Remove Edge")
    {
        Node *node1 = new Node(1, {1.0, 2.0}, {});
        Node *node2 = new Node(2, {3.0, 4.0}, {});
        graph.addNode(node1);
        graph.addNode(node2);
        graph.addEdge(1, node2);
        graph.removeEdge(1, 2);
        REQUIRE(node1->getEdges().empty());
    }

    SECTION("Get Node")
    {
        Node *node = new Node(1, {1.0, 2.0}, {});
        graph.addNode(node);
        REQUIRE(graph.getNode(1) == node);
        REQUIRE(graph.getNode(2) == nullptr);
    }

    // Clean up dynamically allocated nodes
    graph.clear();
}

TEST_CASE("Graph Union - Ownership Transfer", "[GraphUnion]") {
    Graph graph1, graph2;

    // Create nodes for graph2
    Node* node1 = new Node(1, {1.0, 1.0}, {});
    Node* node2 = new Node(2, {2.0, 2.0}, {});
    node1->addEdge(node2);
    graph2.addNode(node1);
    graph2.addNode(node2);

    graph1.graphUnion(move(graph2));

    // Check nodes are transferred
    REQUIRE(graph1.getNode(1) != nullptr);
    REQUIRE(graph1.getNode(2) != nullptr);

    // Check edges are intact
    REQUIRE(graph1.getNode(1)->edgeExists(2));

    // Ensure graph2 is cleared
    REQUIRE(graph2.getAdjList().empty());

    graph1.clear();
    graph2.clear();
}

TEST_CASE("Graph Union - Merge with Overlapping Nodes", "[GraphUnion]") {
    Graph graph1, graph2;

    // Add overlapping node to both graphs
    Node* node1_1 = new Node(1, {1.0, 1.0}, {});
    Node* node1_2 = new Node(1, {1.0, 1.0}, {});  // Same ID but different instance
    Node* node2 = new Node(2, {2.0, 2.0}, {});

    graph1.addNode(node1_1);
    graph2.addNode(node1_2);
    graph2.addNode(node2);
    graph2.addEdge(1, node2);

    graph1.graphUnion(move(graph2));

    // Node 1 should not be duplicated
    REQUIRE(graph1.getNode(1) != nullptr);
    REQUIRE(graph1.getAdjList().size() == 2); // Only two unique nodes

    // Edge from node 1 to node 2 should exist
    REQUIRE(graph1.getNode(1)->edgeExists(2));

    // Ensure graph2 is cleared
    REQUIRE(graph2.getAdjList().empty());

    graph1.clear();
    graph2.clear();
}

TEST_CASE("Graph Union - Empty Graph", "[GraphUnion]") {
    Graph graph1, graph2;

    // Add a node to graph1
    Node* node = new Node(1, {1.0, 1.0}, {});
    graph1.addNode(node);

    // union with an empty graph
    graph1.graphUnion(std::move(graph2));

    // Ensure graph1 remains unchanged
    REQUIRE(graph1.getNode(1) != nullptr);
    REQUIRE(graph1.getAdjList().size() == 1);

    // Ensure graph2 is cleared
    REQUIRE(graph2.getAdjList().empty());

    graph1.clear();
    graph2.clear();
}

TEST_CASE("Graph Union - Self-loops Handling", "[GraphUnion]") {
    Graph graph1, graph2;

    // Create a node with a self-loop in graph1
    Node* node1 = new Node(1, {1.0, 1.0}, {});
    node1->addEdge(node1);
    graph1.addNode(node1);

    // Create a second graph and union
    graph1.graphUnion(std::move(graph2));

    // Ensure self-loop is preserved
    REQUIRE(node1->edgeExists(1));

    graph1.clear();
    graph2.clear();
}

TEST_CASE("Graph Intersection Test - Basic", "[IntersectionBasic]")
{
    // Create graph1 with some nodes and edges
    Graph graph1;
    Node *node1 = new Node(1, {0.0, 1.0}, {});
    Node *node2 = new Node(2, {1.0, 1.0}, {});
    graph1.addNode(node1);
    graph1.addNode(node2);
    graph1.addEdge(1, 2);

    // Create graph2 with an overlapping node and an edge
    Graph graph2;
    Node *node2_copy = new Node(2, {1.0, 1.0}, {});
    graph2.addNode(node2_copy);
    Node *node3 = new Node(3, {2.0, 2.0}, {});
    graph2.addNode(node3);
    graph2.addEdge(2, 3);

    // Perform the intersection
    graph1.graphIntersection(graph2);

    SECTION("Intersection retains common nodes")
    {
        REQUIRE(graph1.getNode(2) != nullptr); // Node 2 should still exist
    }

    SECTION("Intersection removes non-common nodes")
    {
        REQUIRE(graph1.getNode(1) == nullptr); // Node 1 should be removed
        REQUIRE(graph1.getNode(3) == nullptr); // Node 3 shouldn't exist in graph1
    }

    SECTION("Intersection retains common edges")
    {
        REQUIRE(graph1.getNode(2)->getEdges().empty()); // No common edges should remain
    }

    graph1.clear();
    graph2.clear();
}

TEST_CASE("Graph Intersection Test - Empty Intersection", "[IntersectionEmpty]")
{
    // Create two graphs with no overlapping nodes or edges
    Graph graph1;
    Node *node1 = new Node(1, {0.0, 1.0}, {});
    graph1.addNode(node1);

    Graph graph2;
    Node *node2 = new Node(2, {1.0, 1.0}, {});
    graph2.addNode(node2);

    // Perform intersection
    graph1.graphIntersection(graph2);

    SECTION("Intersection of disjoint graphs results in empty graph")
    {
        REQUIRE(graph1.getAdjList().empty()); // graph1 should now be empty
    }

    graph1.clear();
    graph2.clear();
}

TEST_CASE("Graph Intersection Test - Self-loops Handling", "[IntersectionSelfLoops]")
{
    Graph graph1;
    Node *node1 = new Node(1, {0.0, 1.0}, {});
    node1->addEdge(node1); // Self-loop
    graph1.addNode(node1);

    Graph graph2;
    Node *node1_copy = new Node(1, {0.0, 1.0}, {});
    node1_copy->addEdge(node1_copy);
    graph2.addNode(node1_copy);

    // Perform the intersection
    graph1.graphIntersection(graph2);

    SECTION("Intersection retains self-loops if they are present in both graphs")
    {
        REQUIRE(graph1.getNode(1) != nullptr);     // Node 1 should still exist
        REQUIRE(graph1.getNode(1)->edgeExists(1)); // Self-loop should be retained
    }

    graph1.clear();
    graph2.clear();
}
