// #include "catch.hpp"
// #include "../include/stitchedVamana.hpp"

// using namespace std;

// TEST_CASE("stitchedVamana function test")
// {
//     vector<vector<double>> coords = {
//         {1, 1.0, 2.0},
//         {1, 2.0, 3.0},
//         {2, 3.0, 4.0},
//         {2, 5.0, 6.0},
//         {3, 7.0, 8.0}
//     };

//     // input parameters
//     set<int> F = {1, 2, 3};
//     double a = 0.5;
//     int L_small = 2;
//     int R_small = 3;
//     int R_stitched = 4;

//     Graph G = stitchedVamana(coords, F, a, L_small, R_small, R_stitched);

//     // SECTION("Validate stitched graph properties") 
//     // {
//     //     REQUIRE_FALSE(G.isEmpty());  // graph should not be empty
//     //     REQUIRE(G.getNodeCount() == coords.size());  // should have all nodes from input

//     //     // check the existence of nodes and labels
//     //     for (int i = 0; i < coords.size(); i++) 
//     //     {
//     //         Node* node = G.getNode(i);
//     //         REQUIRE(node != nullptr);
//     //         REQUIRE(node->getLabel() == static_cast<int>(coords[i][0]));  // validate correct labels
//     //     }

//     //     // validate specific edges for label 1 (optional)
//     //     SECTION("Check connectivity for label 1 nodes") {
//     //         Node* node0 = G.getNode(0);
//     //         Node* node1 = G.getNode(1);
//     //         REQUIRE(node0->edgeExists(1));  // node 0 should have an edge to Node 1
//     //         REQUIRE(node1->edgeExists(0));  // node 1 should have an edge to Node 0
//     //     }
//     // }

//     // SECTION("Empty input test")
//     // {
//     //     vector<vector<double>> emptyCoords;
//     //     Graph emptyGraph = stitchedVamana(emptyCoords, F, a, L_small, R_small, R_stitched);
//     //     REQUIRE(emptyGraph.isEmpty());  // graph should be empty

//     //     emptyGraph.clear();
//     // }

//     // SECTION("No nodes for specific label")
//     // {
//     //     set<int> Fnew = {1, 2, 3, 4}; // label 4 has no nodes
//     //     Graph G = stitchedVamana(coords, Fnew, a, L_small, R_small, R_stitched);
        
//     //     REQUIRE(G.getNodeCount() == coords.size());  // Existing nodes should still be included

//     //     // Ensure label 4 nodes are absent
//     //     auto nodesWithLabel4 = G.findNodesWithLabel(4);
//     //     REQUIRE(nodesWithLabel4.empty());

//     //     G.clear();
//     // }

//     // // SECTION("Single label test")
//     // // {
//     // //     set<int> Fsingle = {1};
//     // //     map<int, Graph> resultGmap = stitchedVamana(coords, Fsingle, a, L_small, R_small, R_stitched);

//     // //     REQUIRE(resultGmap.size() == 1);
//     // //     REQUIRE(resultGmap.find(1) != resultGmap.end());
//     // //     REQUIRE(resultGmap[1].getNodeCount() == 2); // 2 nodes with label 1

//     // //     for (auto &[f, Gf] : resultGmap)
//     // //     {
//     // //         Gf.clear();
//     // //     }
//     // // }

//     // SECTION("Test with many nodes")
//     // {
//     //     vector<vector<double>> manyCoords;
//     //     for (int i = 0; i < 1000; ++i)
//     //     {
//     //         // create some random coords based on i
//     //         manyCoords.push_back({static_cast<double>(i % 10), static_cast<double>(i), static_cast<double>(i + 1)});
//     //     }

//     //     set<int> largeF = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
//     //     Graph G = stitchedVamana(manyCoords, largeF, a, L_small, R_small, R_stitched);
        
//     //     REQUIRE(G.getNodeCount() == 1000);
//     //     for (int f : largeF) {
//     //         auto nodesWithLabel = G.findNodesWithLabel(f);
//     //         REQUIRE_FALSE(nodesWithLabel.empty());  // Ensure nodes for each label exist
//     //     }

//     //     G.clear();
//     // }

//     G.clear();
// }
