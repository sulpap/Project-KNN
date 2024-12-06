#include "catch.hpp"
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

    cout << "Test: Nodes size: " << G.getNodeCount() << " and coords size: " << coords.size() << endl;

    SECTION("Validate stitched graph properties") 
    {
        REQUIRE_FALSE(G.isEmpty()); // graph should not be empty
        REQUIRE(G.getNodeCount() == static_cast<int>(coords.size())); // should have all nodes from input

        // TODO check the existence of nodes and labels
        

        // TODO validate specific edges for label 0 (optional)

                
    }

    SECTION("Empty input test")
    {
        vector<vector<double>> emptyCoords;
        Graph emptyGraph = stitchedVamana(emptyCoords, F, a, L_small, R_small, R_stitched, medoids);
        REQUIRE(emptyGraph.isEmpty());  // graph should be empty

        emptyGraph.clear();
    }

    SECTION("No nodes for specific label")
    {
        set<int> Fnew = {0, 1, 2, 3, 4}; // labels 3&4 have no nodes
        Graph G = stitchedVamana(coords, Fnew, a, L_small, R_small, R_stitched, medoids);
        
        REQUIRE(G.getNodeCount() == static_cast<int>(coords.size()));  // existing nodes should still be included

        // ensure label 4 nodes are absent
        auto nodesWithLabel4 = G.findNodesWithLabel(4);
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
        for (int f : largeF) {
            auto nodesWithLabel = G.findNodesWithLabel(f);
            REQUIRE_FALSE(nodesWithLabel.empty());  // ensure nodes for each label exist
        }

        G.clear();
    }

    G.clear();
}
