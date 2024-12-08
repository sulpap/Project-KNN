#include "catch.hpp"
#include "../include/graph.hpp"
#include "../include/stitchedVamana.hpp"
#include "../include/generate_graph.hpp"

using namespace std;

TEST_CASE("stitchedVamana function test")
{
    vector<vector<double>> coords = {
        {0, 1.0, 2.0, 3.0},
        {1, 4.0, 5.0, 6.0},
        {1, 7.0, 8.0, 9.0},
        {0, 10.0, 11.0, 12.0},
        {1, 1.5, 2.5, 3.5},
        {2, 4.3, 6.0, 1.9},
        {2, 3.4, 6.1, 1.2}
    };
    set<int> F = {0, 1, 2};

    double a = 1.2;
    int L_small = 5;
    int R_small = 3;
    int R_stitched = 5;
    map<int, Node *> medoids;

    Graph G = stitchedVamana(coords, F, a, L_small, R_small, R_stitched, medoids);

    SECTION("Validate stitched graph properties")
    {
        REQUIRE_FALSE(G.isEmpty());                                   // graph should not be empty
        REQUIRE(G.getNodeCount() == static_cast<int>(coords.size())); // should have all nodes from input

        // validate specific edges for label 0 (example)
        auto nodesWithLabel0 = G.findNodeIdsWithLabel(0);
        for (int nodeId : nodesWithLabel0)
        {
            Node *node = G.getNode(nodeId);
            REQUIRE(node != nullptr);
            for (Node *neighbor : node->getEdges())
            {
                REQUIRE(neighbor->getLabel() == 0); // All neighbors should also have label 0
            }
        }
    }

    SECTION("Empty input test")
    {
        vector<vector<double>> emptyCoords;
        Graph emptyGraph = stitchedVamana(emptyCoords, F, a, L_small, R_small, R_stitched, medoids);
        REQUIRE(emptyGraph.isEmpty()); // graph should be empty

        emptyGraph.clear();
    }

    SECTION("No nodes for specific label")
    {
        set<int> Fnew = {0, 1, 2, 3, 4}; // labels 3&4 have no nodes
        Graph G = stitchedVamana(coords, Fnew, a, L_small, R_small, R_stitched, medoids);

        REQUIRE(G.getNodeCount() == static_cast<int>(coords.size())); // existing nodes should still be included

        // ensure label 4 nodes are absent
        auto nodesWithLabel4 = G.findNodeIdsWithLabel(4);
        REQUIRE(nodesWithLabel4.empty());

        G.clear();
    }

    SECTION("Single label test")
    {
        set<int> Fsingle = {1};
        Graph G = stitchedVamana(coords, Fsingle, a, L_small, R_small, R_stitched, medoids);

        REQUIRE(G.getNodeCount() == 3); // 3 nodes with label 1

        G.clear();
    }

    SECTION("Test with many nodes")
    {
        vector<vector<double>> manyCoords;
        for (int i = 0; i < 1000; ++i)
        {
            // create some random coords based on i
            manyCoords.push_back({static_cast<double>(i % 10), static_cast<double>(i), static_cast<double>(i + 1)});
        }

        set<int> largeF = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        Graph G = stitchedVamana(manyCoords, largeF, a, L_small, R_small, R_stitched, medoids);

        REQUIRE(G.getNodeCount() == 1000);
        for (int f : largeF)
        {
            auto nodesWithLabel = G.findNodeIdsWithLabel(f);
            REQUIRE_FALSE(nodesWithLabel.empty()); // ensure nodes for each label exist
        }

        G.clear();
    }

    G.clear();
}

TEST_CASE("compute_Fx function test in stitchedVamana")
{
    vector<vector<double>> coords = {
        {0, 1.0, 2.0},
        {1, 3.0, 4.0},
        {0, 5.0, 6.0},
    };

    unordered_map<int, set<int>> Fx = compute_Fx(coords);

    // validate the size of Fx matches the number of points
    REQUIRE(Fx.size() == coords.size());

    // validate each point's labels
    for (size_t i = 0; i < coords.size(); i++)
    {
        int expectedLabel = static_cast<int>(coords[i][0]);
        REQUIRE(Fx[i] == set<int>{expectedLabel});
    }
}

TEST_CASE("compute_PfMap function test")
{
    vector<vector<double>> coords = {
        {0, 1.0, 2.0},
        {1, 3.0, 4.0},
        {0, 5.0, 6.0},
        {2, 7.0, 8.0},
    };
    set<int> F = {0, 1, 2};

    unordered_map<int, vector<Node *>> PfMap = compute_PfMap(coords, F);

    // PfMap should have entries for each label in F
    REQUIRE(PfMap.size() == F.size());

    // validate the points corresponding to each label
    REQUIRE(PfMap[0].size() == 2); // two points with label 0
    REQUIRE(PfMap[1].size() == 1); // one point with label 1
    REQUIRE(PfMap[2].size() == 1); // one point with label 2

    // validate coordinates for a specific label (ex. 0)
    vector<int> expectedCoords0 = {0, 2};
    REQUIRE(PfMap[0][0]->getId() == expectedCoords0[0]);
    REQUIRE(PfMap[0][1]->getId() == expectedCoords0[1]);
}

TEST_CASE("store_medoid function test")
{
    Graph G;
    Node *node1 = new Node(1, {0.0, 0.0}, {}, 0);
    Node *node2 = new Node(2, {1.0, 1.0}, {}, 1);
    G.addNode(node1);
    G.addNode(node2);

    map<int, Node *> medoids;
    store_medoid(G, medoids, 0, 1);

    // Validate that the medoid for label 0 is stored correctly
    REQUIRE(medoids.size() == 1);
    REQUIRE(medoids[0] == node1);

    // Test for a non-existent medoid ID
    store_medoid(G, medoids, 1, 3);
    REQUIRE(medoids.size() == 1); // No new medoid should be added for invalid ID

    G.clear();
}
