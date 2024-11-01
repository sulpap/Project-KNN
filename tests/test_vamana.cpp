#include "catch.hpp"
#include "../include/vamana.hpp"

using  namespace std; 

// result of vamana should be a graph whose nodes have edges pointing to their k closest neighbors.
TEST_CASE("Vamana function test") 
{
    // set up a sample graph with nodes
    Graph graph;

    // define the coordinates and parameters
    vector<vector<double>> coords = {
        {1.0, 2.0},
        {2.0, 3.0},
        {3.0, 4.0},
        {4.0, 5.0},
        {2.0, 1.0}
    };
    int k = 2;              // number of closest neighbors needed
    int maxNodesEdges;  // max out-degree (R)
    double a = 1;           // distance threshold

    SECTION("R is a normal number")
    {
        maxNodesEdges = 3;
        Vamana(graph, coords, maxNodesEdges, k, a);

        for (int i = 0; i < graph.getNodeCount(); ++i) 
        {
            Node* node = graph.getNode(i);
            // check degree does not exceed maxNodesEdges
            REQUIRE(node->getEdges().size() <= maxNodesEdges);
        
        }
    }

    SECTION("R is 0")
    {
        maxNodesEdges = 0;
        Vamana(graph, coords, maxNodesEdges, k, a);

        for (int i = 0; i < graph.getNodeCount(); ++i) 
        {
            Node* node = graph.getNode(i);
            // check that the graph remains unchanged - no edges should be added
            REQUIRE(node->getEdges().empty());
        }
        
    }

}
