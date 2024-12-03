#include "catch.hpp"
#include "../include/stitchedVamana.hpp"

using namespace std;

TEST_CASE("stitchedVamana function test")
{
    vector<vector<double>> coords = {
        {1, 1.0, 2.0},
        {1, 2.0, 3.0},
        {2, 3.0, 4.0},
        {2, 5.0, 6.0},
        {3, 7.0, 8.0}
    };

    // input parameters
    set<int> F = {1, 2, 3};
    double a = 0.5;
    int L_small = 2;
    int R_small = 3;
    int R_stitched = 4;

    map<int, Graph> resultGmap = stitchedVamana(coords, F, a, L_small, R_small, R_stitched);

    REQUIRE(resultGmap.size() == F.size()); // we need to have a graph for each label in F

    // validate specific graphs
    for (int f : F)
    {
        REQUIRE(resultGmap.find(f) != resultGmap.end()); // make sure a graph for this label exists
        const Graph &Gf = resultGmap[f];

        REQUIRE_FALSE(Gf.isEmpty()); // the graph must not be empty if nodes exist for this label
        REQUIRE(Gf.getNodeCount() > 0);
    }

    // validate structure and edges for one specific label
    SECTION("Validate graph for label 1")
    {
        const Graph &Gf1 = resultGmap[1];
        REQUIRE(Gf1.getNodeCount() == 2); // label 1 has 2 nodes in input
    }

    SECTION("Empty input test")
    {
        vector<vector<double>> emptyCoords;
        set<int> F = {1, 2, 3};
        map<int, Graph> resultGmap = stitchedVamana(emptyCoords, F, a, L_small, R_small, R_stitched);

        REQUIRE(resultGmap.size() == F.size()); // graphs for all labels should exist 

        for (int f : F)
        {
            REQUIRE(resultGmap[f].isEmpty()); // but they should be empty
        }

        for (auto &[f, Gf] : resultGmap)
        {
            Gf.clear();
        }
    }

    SECTION("No nodes for specific label")
    {
        set<int> Fnew = {1, 2, 3, 4}; // label 4 has no nodes
        map<int, Graph> resultGmap = stitchedVamana(coords, Fnew, a, L_small, R_small, R_stitched);

        REQUIRE(resultGmap.size() == 4); // should create graphs for all labels
        REQUIRE(resultGmap[4].isEmpty()); // but graph of label 4 should be empty

        for (auto &[f, Gf] : resultGmap)
        {
            Gf.clear();
        }
    }

    SECTION("Single label test")
    {
        set<int> Fsingle = {1};
        map<int, Graph> resultGmap = stitchedVamana(coords, Fsingle, a, L_small, R_small, R_stitched);

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
        vector<vector<double>> manyCoords;
        for (int i = 0; i < 1000; ++i)
        {
            // create some random coords based on i
            manyCoords.push_back({static_cast<double>(i % 10), static_cast<double>(i), static_cast<double>(i + 1)});
        }

        set<int> largeF = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        map<int, Graph> resultGmap = stitchedVamana(manyCoords, largeF, a, L_small, R_small, R_stitched);

        REQUIRE(resultGmap.size() == largeF.size());
        for (int f : largeF)
        {
            REQUIRE(resultGmap[f].getNodeCount() > 0);
        }

        for (auto &[f, Gf] : resultGmap)
        {
            Gf.clear();
        }
    }

    for (auto &[f, Gf] : resultGmap)
    {
        Gf.clear();
    }
}