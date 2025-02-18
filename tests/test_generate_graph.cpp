#include "catch.hpp"
#include "../include/graph.hpp"
#include "../include/generate_graph.hpp"
#include <vector>
#include <algorithm>
#include <unordered_set>

using namespace std;

TEST_CASE("Test generate_graph basic functionality")
{
    // example for 3 nodes with max edges 2 each
    vector<vector<double>> coords = {
        {1.0, 2.0, 3.0},
        {4.0, 5.0, 6.0},
        {7.0, 8.0, 9.0}
    };

    vector<Node *> nodes;

    for (size_t i = 0 ; i < coords.size() ; i++) {
        vector<double> point_coords(coords[i]);
        Node *tempNode = new Node(i, point_coords, {}, 0);
        nodes.push_back(tempNode);
    }

    int R = 2;

    Graph graph;
    generate_graph(graph, nodes, R);

    // check that 3 nodes have been added
    int nodecount = graph.getNodeCount();
    REQUIRE(nodecount == 3);
    
    // verify that nodes have the correct coordinates
    REQUIRE(graph.getNode(0)->getCoordinates() == coords[0]);
    REQUIRE(graph.getNode(1)->getCoordinates() == coords[1]);
    REQUIRE(graph.getNode(2)->getCoordinates() == coords[2]);

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
        {4.0, 4.0, 4.0}
    };

    vector<Node *> nodes;

    for (size_t i = 0 ; i < coords.size() ; i++) {
        vector<double> point_coords(coords[i]);
        Node *tempNode = new Node(i, point_coords, {}, -1);
        nodes.push_back(tempNode);
    }

    int R = 3;

    Graph graph;
    generate_graph(graph, nodes, R);

    // verify that there are no self-loops
    for (int i = 0; i < 4; ++i)
    {
        Node *node = graph.getNode(i);
        for (auto edge : node->getEdges())
        {
            REQUIRE(edge->getId() != i); // node should not have an edge to itself
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
        {0, 2.0, 3.0},
        {0, 5.0, 6.0},
        {1, 8.0, 9.0},
        {1, 11.0, 12.0},
        {0, 14.0, 15.0} 
    };

    vector<Node *> nodes;

    // initialize nodes with labels as the first element of coords
    for (size_t i = 0; i < coords.size(); ++i)
    {
        int label = static_cast<int>(coords[i][0]); // take the label from the first element
        vector<double> point_coords(coords[i].begin() + 1, coords[i].end()); // the remaining are coordinates
        Node *tempNode = new Node(i, point_coords, {}, label);
        nodes.push_back(tempNode);
    }

    Graph graph;
    for (Node *node : nodes)
    {
        graph.addNode(node);
    }

    generate_label_based_graph(graph, coords);

    // verify that nodes are connected only to nodes with the same label
    for (size_t i = 0; i < coords.size(); ++i)
    {
        Node *node = graph.getNode(i);
        REQUIRE(node != nullptr); // check that the node exists
        int label = node->getLabel();

        for (auto edge : node->getEdges())
        {
            Node *neighbor = graph.getNode(edge->getId());
            REQUIRE(neighbor != nullptr); // neighbor exists
            REQUIRE(neighbor->getLabel() == label); // and it has the same label
        }
    }

    graph.clear();
}

TEST_CASE("Test generate_random_edges basic functionality")
{
    // create example graph with 4 nodes
    vector<vector<double>> coords = {
        {1.0, 2.0, 3.0},
        {4.0, 5.0, 6.0},
        {7.0, 8.0, 9.0},
        {10.0, 11.0, 12.0}
    };

    vector<Node *> nodes;
    for (size_t i = 0; i < coords.size(); ++i)
    {
        Node *tempNode = new Node(i, coords[i], {}, 0);
        nodes.push_back(tempNode);
    }

    Graph graph;
    for (Node *node : nodes)
    {
        graph.addNode(node);
    }

    int maxEdgesPerNode = 2;
    generate_random_edges(graph, maxEdgesPerNode);

    // verify that each node has at most 2 edges
    for (size_t i = 0; i < nodes.size(); ++i)
    {
        Node *node = graph.getNode(i);
        REQUIRE(node != nullptr); // node exists

        int edgeCount = node->getEdges().size();
        REQUIRE(edgeCount <= maxEdgesPerNode); // node has at most maxEdgesPerNode edges
    }

    SECTION("No self loops")
    {
        for (size_t i = 0; i < nodes.size(); ++i)
        {
            Node *node = graph.getNode(i);
            for (auto edge : node->getEdges())
            {
                REQUIRE(edge->getId() != static_cast<int>(i)); // node should not have an edge to itself
            }
        }
    }

    SECTION("No duplicate edges")
    {
        for (size_t i = 0; i < nodes.size(); ++i)
        {
            Node *node = graph.getNode(i);
            vector<int> edgeIds;
            for (auto edge : node->getEdges())
            {
                edgeIds.push_back(edge->getId());
            }

            // sort in order for duplicates to appear next to each other
            sort(edgeIds.begin(), edgeIds.end());
            // check if there are any
            REQUIRE(adjacent_find(edgeIds.begin(), edgeIds.end()) == edgeIds.end());
        }
    }

    graph.clear();
}

TEST_CASE("Test connect_subgraphs basic functionality")
{
    Graph G;

    // create subgraph 1
    vector<Node *> subgraph1 = {
        new Node(0, {1.0, 2.0, 3.0}, {}, 0),
        new Node(1, {4.0, 5.0, 6.0}, {}, 0)};

    // create subgraph 2
    vector<Node *> subgraph2 = {
        new Node(2, {7.0, 8.0, 9.0}, {}, 1),
        new Node(3, {10.0, 11.0, 12.0}, {}, 1)};

    // create subgraph 3
    vector<Node *> subgraph3 = {
        new Node(4, {13.0, 14.0, 15.0}, {}, 2),
        new Node(5, {16.0, 17.0, 18.0}, {}, 2)};
   

    // add all nodes to the global graph
    for (Node *node : subgraph1) {
        G.addNode(node);
    }
    for (Node *node : subgraph2) {
        G.addNode(node);
    }    
    for (Node *node : subgraph3) {
        G.addNode(node);
    }

    // map subgraphs by their labels
    unordered_map<int, vector<Node *>> PfMap = {
        {0, subgraph1},
        {1, subgraph2},
        {2, subgraph3}};

    connect_subgraphs(G, PfMap);

    // check that the global graph connects the subgraphs --> there should be at least two edges between different subgraphs
    int crossEdges = 0;
    for (auto &[label, nodes] : PfMap)
    {
        for (Node *node : nodes)
        {
            for (auto edge : node->getEdges())
            {
                Node *neighbor = G.getNode(edge->getId());
                REQUIRE(neighbor != nullptr); // Ensure neighbor exists
                if (neighbor->getLabel() != node->getLabel())
                {
                    ++crossEdges;
                }
            }
        }
    }

    REQUIRE(crossEdges >= 2); // at least two edges connect distinct subgraphs
    
    G.clear();
}
