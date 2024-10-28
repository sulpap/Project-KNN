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

    Graph graph;
    generate_graph(graph, coords);

    // check that 3 nodes have been added
    REQUIRE(graph.getNodeCount() == 3);

    // verify that nodes have the correct coordinates
    REQUIRE(graph.getNode(1)->getCoordinates() == coords[0]);
    REQUIRE(graph.getNode(2)->getCoordinates() == coords[1]);
    REQUIRE(graph.getNode(3)->getCoordinates() == coords[2]);

    // ensure that each node has between 1 and maxNodeEdges edges
    for (int i = 1; i <= 3; ++i) {
        Node* node = graph.getNode(i);
        int edgeCount = node->getEdges().size();
        REQUIRE(edgeCount >= 1);
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

    Graph graph;
    generate_graph(graph, coords);

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

        // sort in order for duplicates to appear next to each other
        sort(edges.begin(), edges.end());
        // find the first occurrence of two consecutive identical elements in a range *
        REQUIRE(adjacent_find(edges.begin(), edges.end()) == edges.end());
    }
}

// * if adjacent_find returns edges.end(), it means no consecutive duplicate elements were found, 
// so there are no duplicates in the sorted edges vector.
// if it returns an iterator other than edges.end(), there are duplicate elements.
