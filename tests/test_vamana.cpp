#include "catch.hpp"
#include "../include/vamana.hpp"
#include "../include/greedysearch.hpp"
#include "../include/robustprune.hpp"
#include "../include/generate_graph.hpp"
#include "../include/utility.hpp"

using  namespace std; 

TEST_CASE("Test Vamana") 
{
    SECTION("Basic case with small input") 
    {
        vector<std::vector<double>> coords = {
            {0.0, 0.0},
            {1.0, 1.0},
            {2.0, 2.0},
        };
        int maxNodesEdges = 2;
        int k = 1;
        int a = 1;
        
        Graph graph;
        Vamana(graph, coords, maxNodesEdges, k, a);

        // Validate expected output, like checking node degrees, neighbors, etc.
        // Example: Check if the graph is connected as expected
        for (int i = 1; i <= graph.getNodeCount(); ++i) {
            Node* node = graph.getNode(i);
            REQUIRE(node->getEdges().size() <= maxNodesEdges); // Degree check
        }
    }

}