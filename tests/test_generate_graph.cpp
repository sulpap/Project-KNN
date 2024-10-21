// g++ -std=c++17 -Iinclude -o test_generate_graph tests/test_generate_graph.cpp src/generate_graph.cpp src/graph.cpp

//#define CATCH_CONFIG_MAIN


#include "catch.hpp"
#include "../include/graph.hpp"
#include "../include/generate_graph.hpp"
#include <vector>
#include <algorithm>

using namespace std;

TEST_CASE("Test generate_graph basic functionality") 
{
    // example for 3 nodes with max edges 2 each
    vector<vector<double>> coords = {
        {1.0, 2.0, 3.0},
        {4.0, 5.0, 6.0},
        {7.0, 8.0, 9.0}
    };

    int maxNodeEdges = 2;

    Graph graph;
    
    generate_graph(graph, coords, maxNodeEdges);

    // check that 3 nodes have been added
    REQUIRE(graph.getNodeCount() == 3);

    // verify that nodes have the correct coordinates
    Node* node1 = graph.getNode(1);
    REQUIRE(node1->getCoordinates() == coords[0]);

    Node* node2 = graph.getNode(2);
    REQUIRE(node2->getCoordinates() == coords[1]);

    Node* node3 = graph.getNode(3);
    REQUIRE(node3->getCoordinates() == coords[2]);

    // ensure that each node has between 1 and maxNodeEdges edges
    for (int i = 1; i <= 3; ++i) {
        Node* node = graph.getNode(i);
        int edgeCount = node->getEdges().size();
        REQUIRE(edgeCount >= 1);
        REQUIRE(edgeCount <= maxNodeEdges);
    }
}

TEST_CASE("Test generate_graph no self-loops or duplicate edges") 
{
    // example with 4 nodes with max edges 3 each
    vector<vector<double>> coords = {
        {1.0, 1.0, 1.0},
        {2.0, 2.0, 2.0},
        {3.0, 3.0, 3.0},
        {4.0, 4.0, 4.0}
    };

    int maxNodeEdges = 3;

    Graph graph;

    generate_graph(graph, coords, maxNodeEdges);

    // verify that there are no self-loops
    for (int i = 1; i <= 4; ++i) {
        Node* node = graph.getNode(i);
        for (auto edge : node->getEdges()) {
            REQUIRE(edge->getId() != i);  // Node should not have an edge to itself
        }
    }

    // verify that no duplicate edges exist
    for (int i = 1; i <= 4; ++i) {
        Node* node = graph.getNode(i);
        vector<int> edges;
        
        // collect edge IDs into a vector
        for (auto edge : node->getEdges()) {
            edges.push_back(edge->getId());
        }

        // check if there are duplicate neighbors
        sort(edges.begin(), edges.end());
        REQUIRE(adjacent_find(edges.begin(), edges.end()) == edges.end());
    }
}
