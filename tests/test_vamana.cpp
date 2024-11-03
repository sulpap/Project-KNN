#include "catch.hpp"
#include "../include/vamana.hpp"
#include "../include/utility.hpp"

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
    int R = 3;           // max out-degree (R)
    double a = 1;        // distance threshold
    int int_L = 5;

    int med = Vamana(graph, coords, R, a, int_L);
    int med_check = findMedoid(coords);

    REQUIRE(med == med_check);

    for (int i = 0; i < graph.getNodeCount(); ++i) 
    {
        Node* node = graph.getNode(i);
        // check degree does not exceed R
        REQUIRE(node->getEdges().size() <= R);    
    }

    graph.clear();
}
