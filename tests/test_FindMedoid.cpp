#include "catch.hpp"
#include "../include/FindMedoid.hpp"

TEST_CASE("Test FindMedoid")
{
    // example graph
    Node *node1 = new Node(1, {0.0, 1.0}, {}, 1);
    Node *node2 = new Node(2, {1.0, 2.0}, {}, 1);
    Node *node3 = new Node(3, {2.0, 3.0}, {}, 2);
    Node *node4 = new Node(4, {3.0, 4.0}, {}, 2);
    Node *node5 = new Node(5, {4.0, 5.0}, {}, 3);

    Graph graph;
    graph.addNode(node1);
    graph.addNode(node2);
    graph.addNode(node3);
    graph.addNode(node4);
    graph.addNode(node5);

    SECTION("Basic input")
    {
        set<int> F = {1, 2, 3};
        int taph = 2;

        map<int, Node *> medoids = FindMedoid(graph, taph, F);

        REQUIRE(medoids.size() == 3); // should return one medoid for each label

        for (int label : F)
        {
            REQUIRE(medoids.find(label) != medoids.end()); // Each label has a medoid
            REQUIRE(medoids[label] != nullptr);            // Medoid should not be null
        }
    }

    SECTION("Empty label set")
    {
        set<int> F; // empty set
        int taph = 2;

        map<int, Node *> medoids = FindMedoid(graph, taph, F);
        REQUIRE(medoids.empty()); // should return no medoids
    }

    SECTION("Empty graph")
    {
        Graph emptyGraph;

        set<int> F = {1, 2, 3}; // Arbitrary labels
        int taph = 1;

        map<int, Node *> medoids = FindMedoid(emptyGraph, taph, F);
        REQUIRE(medoids.empty());
    }

    SECTION("Label not in graph")
    {
        set<int> F = {1, 8}; // 8 doesn't exist as a label
        int taph = 2;

        map<int, Node *> medoids = FindMedoid(graph, taph, F);
        REQUIRE(medoids.size() == 1);              // only one medoid for label 1
        REQUIRE(medoids.find(8) == medoids.end()); // no medoid for non-existent label
    }

    SECTION("Single node graph with matching label")
    {
        Node *node = new Node(1, {0.0, 1.0}, {}, 1);
        Graph graph;
        graph.addNode(node);

        set<int> F = {1}; // Label matches
        int taph = 1;     // Sampling 1 node is fine

        map<int, Node *> medoids = FindMedoid(graph, taph, F);
        REQUIRE(medoids.size() == 1);
        REQUIRE(medoids[1]->getId() == 1);

        graph.clear();
    }

    SECTION("Single node graph with non-matching label")
    {
        Node *node = new Node(1, {0.0, 1.0}, {}, 2);
        Graph graph;
        graph.addNode(node);

        set<int> F = {1}; // No match for label
        int taph = 1;

        map<int, Node *> medoids = FindMedoid(graph, taph, F);
        REQUIRE(medoids.empty());

        graph.clear();
    }

    SECTION("Duplicate labels")
    {
        Node *node1 = new Node(1, {0.0, 1.0}, {}, 1);
        Node *node2 = new Node(2, {1.0, 2.0}, {}, 1);
        Node *node3 = new Node(3, {2.0, 3.0}, {}, 2);
        Graph graph;
        graph.addNode(node1);
        graph.addNode(node2);
        graph.addNode(node3);

        set<int> F = {1};
        int taph = 2; // to include both nodes with label 1

        map<int, Node *> medoids = FindMedoid(graph, taph, F);
        REQUIRE(medoids.size() == 1); // should be one medoid from the nodes sharing the label
        REQUIRE((medoids[1]->getId() == 1 || medoids[1]->getId() == 2));

        graph.clear();
    }

    SECTION("Graph with disconnected nodes")
    {
        Node *node1 = new Node(1, {0.0, 1.0}, {}, 1);
        Node *node2 = new Node(2, {1.0, 2.0}, {}, 2);
        Node *node3 = new Node(3, {2.0, 3.0}, {}, 3);
        Graph graph;
        graph.addNode(node1);
        graph.addNode(node2);
        graph.addNode(node3);

        set<int> F = {1, 2, 3};
        int taph = 1;

        map<int, Node *> medoids = FindMedoid(graph, taph, F);
        REQUIRE(medoids.size() == 3);
        REQUIRE(medoids[1]->getId() == 1);
        REQUIRE(medoids[2]->getId() == 2);
        REQUIRE(medoids[3]->getId() == 3);

        graph.clear();
    }

    SECTION("Large graph with high taph value")
    {
        Graph graph;
        for (int i = 1; i <= 100; ++i)
        {
            graph.addNode(new Node(i, {static_cast<double>(i)}, {}, i % 5)); // Labels: 0-4
        }

        // set<int> F = {1, 2, 3};
        // int taph = 50; // Exceeds nodes with label

        // map<int, Node *> medoids = FindMedoid(graph, taph, F);
        // REQUIRE(medoids.size() == 3);

        graph.clear();
    }

    SECTION("Graph with identical coordinates but different labels")
    {
        Node *node1 = new Node(1, {1.0, 1.0}, {}, 1);
        Node *node2 = new Node(2, {1.0, 1.0}, {}, 2);
        Node *node3 = new Node(3, {1.0, 1.0}, {}, 3);
        Graph graph;
        graph.addNode(node1);
        graph.addNode(node2);
        graph.addNode(node3);

        set<int> F = {1, 2, 3};
        int taph = 1;

        // FindMedoid should select medoids based on labels, not coords
        map<int, Node *> medoids = FindMedoid(graph, taph, F);
        REQUIRE(medoids.size() == 3);
        REQUIRE(medoids[1]->getId() == 1);
        REQUIRE(medoids[2]->getId() == 2);
        REQUIRE(medoids[3]->getId() == 3);

        graph.clear();
    }

    graph.clear();
}
