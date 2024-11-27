#include "catch.hpp"
#include "../include/graph.hpp"
#include "../include/graph_binary_io.hpp"
#include <fstream>
#include <vector>

using namespace std;

// helper function - checks if two graphs are equal
bool graphs_are_equal(Graph &g1, Graph &g2)
{
    if (g1.getGraphId() != g2.getGraphId() || g1.getNodeCount() != g2.getNodeCount())
    {
        return false;
    }

    auto adjList1 = g1.getAdjList();
    auto adjList2 = g2.getAdjList();

    for (auto &[id, node1] : adjList1)
    {
        Node *node2 = g2.getNode(id);
        if (!node2 || node1->getLabel() != node2->getLabel() || node1->getCoordinates() != node2->getCoordinates())
        {
            return false;
        }

        auto edges1 = node1->getNeighbors();
        auto edges2 = node2->getNeighbors();
        if (edges1 != edges2)
        {
            return false;
        }
    }
    
    adjList1.clear(); 
    adjList2.clear();
    
    return true;
}

TEST_CASE("Save and Load Graph - Basic Test")
{
    // example graph
    Graph graph;

    Node *node1 = new Node(1, {0.0, 1.0}, {}, 10);
    Node *node2 = new Node(2, {2.0, 3.0}, {}, 20);
    Node *node3 = new Node(3, {4.0, 5.0}, {}, 30);

    node1->addEdge(node2);
    node2->addEdge(node3);

    graph.addNode(node1);
    graph.addNode(node2);
    graph.addNode(node3);

    // save the graph to a binary file
    string filename = "test_graph.bin";
    save_graph_to_binary(graph, filename);

    // load the graph back
    Graph loadedGraph = load_graph_from_binary(filename);

    // compare the original and loaded graphs
    REQUIRE(graphs_are_equal(graph, loadedGraph));

    // clean up
    remove(filename.c_str());
    graph.clear();
    loadedGraph.clear();
}

TEST_CASE("Save and Load Graph - Empty Graph")
{
    // initialize an empty graph
    Graph emptyGraph;

    // store it to a binary file
    string filename = "empty_graph.bin";
    save_graph_to_binary(emptyGraph, filename);

    // load it back
    Graph loadedGraph = load_graph_from_binary(filename);

    // verify that the loaded graph is empty and matches the original
    REQUIRE(graphs_are_equal(emptyGraph, loadedGraph));

    // clean up
    remove(filename.c_str());
    emptyGraph.clear();
    loadedGraph.clear();
}

TEST_CASE("Save and Load Graph - Complex Graph")
{
    // create a more complex graph
    Graph graph;
    Node *node1 = new Node(1, {1.1, 1.2, 1.3}, {}, 1);
    Node *node2 = new Node(2, {2.1, 2.2, 2.3}, {}, 2);
    Node *node3 = new Node(3, {3.1, 3.2, 3.3}, {}, 3);
    Node *node4 = new Node(4, {4.1, 4.2, 4.3}, {}, 4);

    node1->addEdge(node2);
    node2->addEdge(node3);
    node3->addEdge(node4);
    node4->addEdge(node1); // circular connection

    graph.addNode(node1);
    graph.addNode(node2);
    graph.addNode(node3);
    graph.addNode(node4);

    // save the graph to a binary file
    string filename = "complex_graph.bin";
    save_graph_to_binary(graph, filename);

    // load the graph back
    Graph loadedGraph = load_graph_from_binary(filename);

    // compare the original and loaded graphs
    REQUIRE(graphs_are_equal(graph, loadedGraph));

    // clean up
    remove(filename.c_str());
    graph.clear();
    loadedGraph.clear();
}
