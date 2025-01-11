#include "catch.hpp"
#include "../include/vamana.hpp"
#include "../include/generate_graph.hpp"
#include "../include/utility.hpp"

using namespace std;

// result of vamana should be a graph whose nodes have edges pointing to their k closest neighbors.
TEST_CASE("Vamana function test")
{
    // set up a sample graph with nodes
    Graph graph;
    vector<Node *> nodes;

    // define the coordinates and parameters
    vector<vector<double>> coords = {
        {1.0, 2.0},
        {2.0, 3.0},
        {3.0, 4.0},
        {4.0, 5.0},
        {2.0, 1.0}
    };

    for (size_t i = 0 ; i < coords.size() ; i++) {
        vector<double> point_coords(coords[i]);
        Node *tempNode = new Node(i, point_coords, {}, -1);
        nodes.push_back(tempNode);
    }

    int R = 3;    // max out-degree (R)
    double a = 1; // distance threshold
    int int_L = 5;

    int med = Vamana(graph, nodes, R, a, int_L);
    int med_check = findMedoid(coords);

    REQUIRE(med == med_check);

    int nodecount = graph.getNodeCount();

    for (int i = 0; i < nodecount; ++i)
    {
        Node *node = graph.getNode(i);
        // check degree does not exceed R
        REQUIRE(node->getEdges().size() <= static_cast<size_t>(R));
    }

    graph.clear();
}
