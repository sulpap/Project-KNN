#include "catch.hpp"
#include "../include/stitchedVamana.hpp"
#include "../include/generate_graph.hpp"

using namespace std;

TEST_CASE("stitchedVamana function test")
{
    vector<Node *> nodes;

    nodes.push_back(new Node(0, {1.0, 2.0}, {}, 1));
    nodes.push_back(new Node(1, {2.0, 3.0}, {}, 1));
    nodes.push_back(new Node(2, {3.0, 4.0}, {}, 2));
    nodes.push_back(new Node(3, {5.0, 6.0}, {}, 2));
    nodes.push_back(new Node(4, {7.0, 8.0}, {}, 3));

    // input parameters
    set<int> F = {1, 2, 3};
    double a = 0.5;
    int L_small = 2;
    int R_small = 3;
    int R_stitched = 4;

    map<int, Graph> resultGmap = stitchedVamana(nodes, F, a, L_small, R_small, R_stitched);

    REQUIRE(resultGmap.size() == F.size()); // we need to have a graph for each label in F

    // validate specific graphs
    for (int f : F)
    {
        REQUIRE(resultGmap.find(f) != resultGmap.end()); // make sure a graph for this label exists
        const Graph &Gf = resultGmap[f];

        REQUIRE(!Gf.isEmpty()); // the graph must not be empty if nodes exist for this label
        REQUIRE(Gf.getNodeCount() > 0);
    }

    // validate structure and edges for one specific label
    SECTION("Validate graph for label 1")
    {
        const Graph &Gf1 = resultGmap[1];
        REQUIRE(Gf1.getNodeCount() == 2); // label 1 has 2 nodes in input

        Node *node0 = Gf1.getNode(1 * OFFSET + 0);
        Node *node1 = Gf1.getNode(1 * OFFSET + 1);
        REQUIRE(node0 != nullptr);
        REQUIRE(node1 != nullptr);
    }

    SECTION("Empty input test")
    {
        vector<Node *> emptyNodes;
        set<int> F = {1, 2, 3};
        map<int, Graph> resultGmap = stitchedVamana(emptyNodes, F, a, L_small, R_small, R_stitched);

        REQUIRE(resultGmap.size() == 0); // we don't have output. a warning message is printed.

        for (auto &[f, Gf] : resultGmap)
        {
            Gf.clear();
        }

        for (Node *node : emptyNodes)
        {
            delete node;
        }
    }

    SECTION("No nodes for specific label")
    {
        set<int> Fnew = {1, 2, 3, 4}; // label 4 has no nodes
        map<int, Graph> resultGmap = stitchedVamana(nodes, Fnew, a, L_small, R_small, R_stitched);

        REQUIRE(resultGmap.size() == 3); // should create graphs for all labels apart from 4

        for (auto &[f, Gf] : resultGmap)
        {
            Gf.clear();
        }
    }

    SECTION("Single label test")
    {
        set<int> Fsingle = {1};
        map<int, Graph> resultGmap = stitchedVamana(nodes, Fsingle, a, L_small, R_small, R_stitched);

        REQUIRE(resultGmap.size() == 1);
        REQUIRE(resultGmap.find(1) != resultGmap.end());
        REQUIRE(resultGmap[1].getNodeCount() == 2); // 2 nodes with label 1

        for (auto &[f, Gf] : resultGmap)
        {
            Gf.clear();
        }
    }

    SECTION("Test with many nodes")
    {
        vector<Node *> nodes;
        for (int i = 0; i < 1000; ++i)
        {
            nodes.push_back(new Node(i, {static_cast<double>(i), static_cast<double>(i + 1)}, {}, i % 10)); // make nodes with label 0-9
        }

        set<int> largeF = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        map<int, Graph> resultGmap = stitchedVamana(nodes, largeF, a, L_small, R_small, R_stitched);

        REQUIRE(resultGmap.size() == largeF.size());
        for (int f : largeF)
        {
            REQUIRE(resultGmap[f].getNodeCount() > 0);
        }

        for (auto &[f, Gf] : resultGmap)
        {
            Gf.clear();
        }

        for (Node *node : nodes)
        {
            delete node;
        }
    }

    for (auto &[f, Gf] : resultGmap)
    {
        Gf.clear();
    }

    for (Node *node : nodes)
    {
        delete node;
    }
}