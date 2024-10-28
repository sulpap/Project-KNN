#include "catch.hpp"
#include "../include/vamana.hpp"
#include "../include/greedysearch.hpp"
#include "../include/robustprune.hpp"
#include "../include/generate_graph.hpp"
#include "../include/utility.hpp"

using  namespace std; 

// TEST_CASE("Test Vamana") 
// {
//     SECTION("Basic case with small input") 
//     {
//         // sample input
//         int numNodes = 5;
//         vector<vector<double>> coords = {
//             {0.0, 0.0},
//             {1.0, 1.0},
//             {2.0, 2.0},
//             {3.0, 3.0},
//             {4.0, 4.0}
//         };
//         int maxNodesEdges = 2;
//         int k = 2;
//         int a = 1;
        
//         Graph graph;
//         Vamana(graph, coords, maxNodesEdges, k, a);
        
//         // check if graph is not empty
//         REQUIRE(graph.getNodeCount() > 0);

//         for (int i = 1; i <= graph.getNodeCount(); ++i) {
//             Node* node = graph.getNode(i);
//             REQUIRE(node->getEdges().size() <= maxNodesEdges); 
//         }   

//         // follow vamana manually for another graph and see if the results are the same.
//         Graph graph2;
//         Node* s = graph.getNode(2); // i = 2 is the medoid in this case.


//     }

// }

// result of vamana should be a graph whose nodes have edges pointing to their k closest neighbors.
TEST_CASE("Vamana function test") 
{
    // set up a sample graph with nodes
    Graph graph;
    Node* n1 = new Node(1, {0.0, 0.0}, {});
    Node* n2 = new Node(2, {1.0, 0.0}, {});
    Node* n3 = new Node(3, {3.0, 3.0}, {});
    Node* n4 = new Node(4, {4.0, 4.0}, {});
    
    graph.addNode(n1);
    graph.addNode(n2);
    graph.addNode(n3);
    graph.addNode(n4);

    // define the coordinates and parameters
    vector<vector<double>> coords = {
        {0.0, 0.0}, // Node 1
        {1.0, 0.0}, // Node 2
        {3.0, 3.0}, // Node 3
        {4.0, 4.0}  // Node 4
    };
    int k = 1;              // number of closest neighbors needed
    int maxNodesEdges = 2;  // max out-degree (R)
    double a = 1.5;         // distance threshold

    Vamana(graph, coords, maxNodesEdges, k, a);

    // after vamana, results should be:
    // Node 1 -> Node 2
    // Node 2 -> Node 1
    // Node 3 -> Node 4
    // Node 4 -> Node 3

    // so, expected edges:
    vector<pair<int, int>> expectedEdges = {
        {1, 2}, // Node 1 to Node 2
        {2, 1}, // Node 2 to Node 1
        {3, 4}, // Node 3 to Node 4
        {4, 3}  // Node 4 to Node 3
    };

    // check each expected edge:
    for (auto& edge : expectedEdges) {
        int from = edge.first;
        int to = edge.second;

        Node* fromNode = graph.getNode(from);
        // ensure the from node exists
        REQUIRE(fromNode != nullptr);
        // check if the 'from' node has an edge pointing to the 'to' node
        REQUIRE(fromNode->edgeExists(to));
    }

    // check expected outcomes
    for (int i = 1; i <= graph.getNodeCount(); i++) {
        Node* currentNode = graph.getNode(i);
        // Ensure the degree of each node does not exceed maxNodesEdges
        REQUIRE(currentNode->getEdges().size() <= static_cast<size_t>(maxNodesEdges));
    }

    // cleaning
    delete n1;
    delete n2;
    delete n3;
    delete n4;
}