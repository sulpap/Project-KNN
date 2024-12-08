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
    string path = "datasets/smallscale/" + filename;
    remove(path.c_str());
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
    string path = "datasets/smallscale/" + filename;
    remove(path.c_str());
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
    string path = "datasets/smallscale/" + filename;
    remove(path.c_str());
    graph.clear();
    loadedGraph.clear();
}

TEST_CASE("Save and Load Map - Empty Map")
{
    // Create an empty map
    map<int, Node*> emptyMap;
    Graph dummyGraph;

    // Save the map to a binary file
    string filename = "empty_map.bin";
    save_map_to_binary(emptyMap, filename);

    // Load the map back
    map<int, Node*> loadedMap = load_map_from_binary(filename, dummyGraph);

    // Verify the loaded map is empty
    REQUIRE(loadedMap.empty());

    // clean up
    string path = "datasets/smallscale/" + filename;
    remove(path.c_str());
    dummyGraph.clear();
}

TEST_CASE("Save and Load Map - Single Node Map")
{
    // Create a graph with a single node
    Graph graph;
    Node* node1 = new Node(1, {0.0, 1.0}, {}, 100);
    graph.addNode(node1);

    // Create a map with one entry
    map<int, Node*> mapSingle;
    mapSingle[1] = node1;

    // Save the map to a binary file
    string filename = "single_node_map.bin";
    save_map_to_binary(mapSingle, filename);

    // Load the map back
    map<int, Node*> loadedMap = load_map_from_binary(filename, graph);

    // Verify the loaded map matches the original
    REQUIRE(loadedMap.size() == mapSingle.size());
    REQUIRE(loadedMap[1] == node1);

    // clean up
    string path = "datasets/smallscale/" + filename;
    remove(path.c_str());
    graph.clear();
}

TEST_CASE("Save and Load Map - Multiple Nodes Map")
{
    // create a graph with multiple nodes
    Graph graph;
    Node* node1 = new Node(1, {1.0, 1.1}, {}, 101);
    Node* node2 = new Node(2, {2.0, 2.1}, {}, 102);
    Node* node3 = new Node(3, {3.0, 3.1}, {}, 103);
    graph.addNode(node1);
    graph.addNode(node2);
    graph.addNode(node3);

    // create a map with multiple entries
    map<int, Node*> mapMultiple;
    mapMultiple[1] = node1;
    mapMultiple[2] = node2;
    mapMultiple[3] = node3;

    // save & load the map
    string filename = "multiple_nodes_map.bin";
    save_map_to_binary(mapMultiple, filename);
    map<int, Node*> loadedMap = load_map_from_binary(filename, graph);

    // verify the loaded map matches the original
    REQUIRE(loadedMap.size() == mapMultiple.size());
    REQUIRE(loadedMap[1] == node1);
    REQUIRE(loadedMap[2] == node2);
    REQUIRE(loadedMap[3] == node3);
    
    string path = "datasets/smallscale/" + filename;
    remove(path.c_str());
    graph.clear();
}

TEST_CASE("Save and Load Map - Nonexistent Node in Map")
{
    // graph with no nodes
    Graph graph;

    // simulate a corrupted map file referencing a nonexistent node
    string filename = "invalid_map.bin";
    ofstream outFile("datasets/smallscale/" + filename, ios::binary);
    int mapSize = 1;  // 1 entry
    outFile.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize));
    int label = 99;      // label
    int nodeId = 42;     // nonexistent Node ID
    outFile.write(reinterpret_cast<const char*>(&label), sizeof(label));
    outFile.write(reinterpret_cast<const char*>(&nodeId), sizeof(nodeId));
    outFile.close();

    // try to load the corrupted map
    REQUIRE_THROWS_WITH(
        load_map_from_binary(filename, graph),
        Catch::Contains("Node ID not found in graph")
    );

    string path = "datasets/smallscale/" + filename;
    remove(path.c_str());
    graph.clear();
}
