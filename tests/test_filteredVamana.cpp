#include "catch.hpp"
#include "../include/filteredVamana.hpp"

// helper function to check graph structure
bool validate_R(Graph &G, int R)
{
    for (const auto &[id, node] : G.getAdjList())
    {
        if (static_cast<int>(node->getEdges().size()) > R)
        {
            return false;
        }
    }
    return true;
}

TEST_CASE("filteredVamana function test")
{
    vector<vector<double>> coords = {
        {0, 1.0, 2.0}, // point 0 with label 0 and so on
        {1, 2.0, 3.0},
        {0, 1.5, 2.5},
        {1, 3.0, 4.0},
        {2, 5.0, 5.0}};

    double alpha = 1.2;
    int L = 2;
    int R = 2;
    set<int> labels = {0, 1, 2};
    int taph = 3;
    map<int, Node *> st_f;

    Graph G = filteredVamana(coords, alpha, L, R, labels, taph, st_f);

    // check that the graph is not empty
    REQUIRE_FALSE(G.isEmpty());

    // initialization checks and label validation is done in the corresponding function tests (below)

    // check that out-degrees are within the allowed limit
    REQUIRE(validate_R(G, R));

    // check that medoids are correctly identified for each label
    REQUIRE(st_f.size() == labels.size()); // ensure all labels have a medoid
    for (int label : labels)
    {
        REQUIRE(st_f.find(label) != st_f.end());
        REQUIRE(st_f[label] != nullptr);
    }

    // some example additional checks for specific connections
    SECTION("Graph contains expected edges")
    {
        Node *node0 = G.getNode(0);
        REQUIRE(node0 != nullptr);

        auto neighbors0 = node0->getNeighbors();
        REQUIRE_FALSE(neighbors0.empty()); // should have neighbors

        // example check: see if node 0 is connected to node 2 (both have label 0)
        REQUIRE(find(neighbors0.begin(), neighbors0.end(), 2) != neighbors0.end());
    }

    SECTION("Graph properties after pruning")
    {
        // example check: node 3
        Node *node3 = G.getNode(3);
        REQUIRE(node3 != nullptr);
        REQUIRE(static_cast<int>(node3->getEdges().size()) <= R);
    }

    G.clear();
}

TEST_CASE("initialize_graph function test")
{
    vector<vector<double>> coords = {
        {0, 1.0, 2.0},
        {1, 2.0, 3.0},
        {2, 3.0, 4.0},
    };

    Graph G;
    initialize_graph(G, coords);

    // check that the graph is not empty
    REQUIRE_FALSE(G.isEmpty());

    // check that the graph contains the correct number of nodes
    REQUIRE(G.getNodeCount() == static_cast<int>(coords.size()));

    // check that each node has the expected label and coordinates
    for (size_t i = 0; i < coords.size(); ++i)
    {
        Node *node = G.getNode(static_cast<int>(i));
        REQUIRE(node != nullptr);
        REQUIRE(node->getLabel() == static_cast<int>(coords[i][0]));

        vector<double> expectedCoords(coords[i].begin() + 1, coords[i].end());
        REQUIRE(node->getCoordinates() == expectedCoords);
    }

    G.clear();
}

TEST_CASE("compute_Fx function test in filteredVamana")
{
    vector<vector<double>> coords = {
        {0, 1.0, 2.0},
        {1, 2.0, 3.0},
        {0, 1.5, 2.5},
        {2, 5.0, 5.0}
    };
    set<int> labels = {0, 1, 2};
    Graph G;
    initialize_graph(G, coords);
    unordered_map<int, set<int>> Fx = compute_Fx(G, labels);

    // check that Fx contains the correct number of entries
    REQUIRE(static_cast<int>(Fx.size()) == G.getNodeCount());

    // check that each node has the correct label in Fx
    for (const auto &[nodeId, nodePtr] : G.getAdjList())
    {
        REQUIRE(Fx.find(nodeId) != Fx.end());
        int label = nodePtr->getLabel();
        REQUIRE(Fx[nodeId] == set<int>{label});
    }

    G.clear();
}
