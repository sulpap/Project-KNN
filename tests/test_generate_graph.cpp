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
        {7.0, 8.0, 9.0}};
    int R = 2;

    Graph graph;
    generate_graph(graph, coords, R);

    // check that 3 nodes have been added
    int nodecount = graph.getNodeCount();
    REQUIRE(nodecount == 3);

    // verify that nodes have the correct coordinates
    REQUIRE(graph.getNode(0)->getCoordinates() == coords[0]);
    REQUIRE(graph.getNode(1)->getCoordinates() == coords[1]);
    REQUIRE(graph.getNode(2)->getCoordinates() == coords[2]);

    // ensure that each node has R edges
    for (int i = 0; i < 3; ++i)
    {
        Node *node = graph.getNode(i);
        int edgeCount = node->getEdges().size();
        REQUIRE(edgeCount == R);
    }

    graph.clear();
}

TEST_CASE("Test generate_graph no self-loops or duplicate edges")
{
    // example with 4 nodes with max edges 3 each
    vector<vector<double>> coords = {
        {1.0, 1.0, 1.0},
        {2.0, 2.0, 2.0},
        {3.0, 3.0, 3.0},
        {4.0, 4.0, 4.0}};
    int R = 3;

    Graph graph;
    generate_graph(graph, coords, R);

    // verify that there are no self-loops
    for (int i = 0; i < 4; ++i)
    {
        Node *node = graph.getNode(i);
        for (auto edge : node->getEdges())
        {
            REQUIRE(edge->getId() != i); // Node should not have an edge to itself
        }
    }

    // verify that no duplicate edges exist
    for (int i = 0; i < 4; ++i)
    {
        Node *node = graph.getNode(i);
        vector<int> edges;

        // collect edge IDs into a vector
        for (auto edge : node->getEdges())
        {
            edges.push_back(edge->getId());
        }

        // sort in order for duplicates to appear next to each other
        sort(edges.begin(), edges.end());
        // find the first occurrence of two consecutive identical elements in a range *
        REQUIRE(adjacent_find(edges.begin(), edges.end()) == edges.end());
    }

    graph.clear();
}

// * if adjacent_find returns edges.end(), it means no consecutive duplicate elements were found,
// so there are no duplicates in the sorted edges vector.
// if it returns an iterator other than edges.end(), there are duplicate elements.
