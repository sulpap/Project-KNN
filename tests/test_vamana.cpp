#include "catch.hpp"
#include "../include/vamana.hpp"
#include "../include/greedysearch.hpp"
#include "../include/robustprune.hpp"
#include "../include/generate_graph.hpp"
#include "../include/utility.hpp"

TEST_CASE("Test Vamana") 
{
    // sample inputs
    vector<vector<double>> coords = { {0.0, 1.0}, {1.0, 0.0}, {2.0, 1.0}, {1.5, 1.5}, {0.5, 2.0} };
    int maxNodesEdges = 3;
    int k = 2;
    int a = 2;

    Graph graph;
    generate_graph(graph, coords);

    Vamana(coords, maxNodesEdges, k, a);

    SECTION("Graph check") {
        REQUIRE(graph.getNodeCount() == coords.size());

        // verify that each node has degree <= maxNodesEdges
        for (int i = 1; i <= graph.getNodeCount(); ++i) 
        {
            REQUIRE(graph.getNode(i)->getEdges().size() <= maxNodesEdges);
        }
    }

    SECTION("Edge and neighbor checks") {
        for (int i = 1; i <= graph.getNodeCount(); ++i) 
        {
            Node* node = graph.getNode(i);

            //check each neighbor connection if necessary
            for (Node* neighbor : node->getEdges()) {
                REQUIRE(neighbor != nullptr);
                // thelei kati allo? afta exoun elegxthei oti doulevoun k sto graph
            }
        }
    }

    SECTION("Algorithm checks") {
        // TODO
    }

}