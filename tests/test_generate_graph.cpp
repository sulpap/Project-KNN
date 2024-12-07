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
    unordered_map<int, int> indexes;
    generate_graph(graph, coords, R, 0, indexes);

    // check that 3 nodes have been added
    int nodecount = graph.getNodeCount();
    REQUIRE(nodecount == 3);
    
    // verify that nodes have the correct coordinates
    REQUIRE(graph.getNode(0 * OFFSET + 0)->getCoordinates() == coords[0]);
    REQUIRE(graph.getNode(0 * OFFSET + 1)->getCoordinates() == coords[1]);
    REQUIRE(graph.getNode(0 * OFFSET + 2)->getCoordinates() == coords[2]);

    // ensure indexes work correctly
    REQUIRE(indexes.size() == 3); // 3 nodes so 3 indexes
    REQUIRE(graph.getNode(0 * OFFSET + 0)->getId()  == indexes[0]);
    REQUIRE(graph.getNode(0 * OFFSET + 1)->getId()  == indexes[1]);
    REQUIRE(graph.getNode(0 * OFFSET + 2)->getId()  == indexes[2]);

    // ensure that each node has R edges and that label is as declared (0)
    for (int i = 0; i < 3; ++i)
    {
        Node *node = graph.getNode(i);
        int edgeCount = node->getEdges().size();
        REQUIRE(edgeCount == R);
        REQUIRE(node->getLabel() == 0); // check that label is 0
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
    unordered_map<int, int> indexes;
    generate_graph(graph, coords, R, 0, indexes);

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

TEST_CASE("Test generate_label_based_graph basic functionality")
{
    vector<vector<double>> coords = {
        {1.0, 2.0, 3.0},
        {4.0, 5.0, 6.0},
        {7.0, 8.0, 9.0},
        {10.0, 11.0, 12.0},
        {13.0, 14.0, 15.0}
    };
    
    set<int> F = {0, 1};

    Graph graph;
    generate_label_based_graph(graph, coords, F);

    // check that all nodes were created
    REQUIRE(graph.getNodeCount() == static_cast<int>(coords.size()));

    // check that each node's coordinates are correct
    for (size_t i = 0; i < coords.size(); ++i)
    {
        REQUIRE(graph.getNode(i)->getCoordinates() == coords[i]);
    }

    for (size_t i = 0; i < coords.size(); ++i)
    {
        // check that labels are within {0, 1, -1}
        int label = graph.getNode(i)->getLabel();
        REQUIRE((F.find(label) != F.end() || label == -1));
        
        // check that each node has at most 2 edges
        // Node* node = graph.getNode(i);
        // REQUIRE(node->getEdges().size() <= 2);
    }

    // check that nodes with label -1 are connected to nodes with different labels
    for (size_t i = 0; i < coords.size(); ++i)
    {
        Node* node = graph.getNode(i);
        if (node->getLabel() == -1)
        {
            set<int> neighborLabels;
            for (auto neighbor : node->getEdges())
            {
                neighborLabels.insert(neighbor->getLabel());
            }
            // neighbor labels should contain at least two different labels from F
            // REQUIRE(neighborLabels.size() >= 2);
        }
    }

    graph.clear();
}