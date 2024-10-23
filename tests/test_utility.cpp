// g++ -std=c++17 -Iinclude -o test_utility tests/test_utility.cpp src/utility.cpp src/graph.cpp

#include "catch.hpp"
#include "../include/graph.hpp"  
#include "../include/utility.hpp"
#include <cmath>
#include <vector>
#include <list>

using namespace std;

TEST_CASE("Test euclidean_distance") 
{
    vector<double> coords1 = {0.0, 0.0, 0.0};
    vector<double> coords2 = {3.0, 4.0, 0.0};

    SECTION("Test distance between (0,0,0) and (3,4,0)") {
        double expected_distance = 5.0; // 3^2 + 4^2 = 25, sqrt(25) = 5
        REQUIRE(euclidean_distance(coords1, coords2) == Approx(expected_distance).epsilon(0.01));
    }

    SECTION("Test distance between identical points") {
        REQUIRE(euclidean_distance(coords1, coords1) == Approx(0.0).epsilon(0.01));
    }
}

TEST_CASE("Test euclidean_distance_of_nodes") 
{
    //empty edge lists
    list<Node*> empty_edges;

    Node node1(1, {0.0, 0.0, 0.0}, empty_edges);
    Node node2(2, {3.0, 4.0, 0.0}, empty_edges);

    SECTION("Test distance between two nodes (0,0,0) and (3,4,0)") {
        double expected_distance = 5.0; // 3^2 + 4^2 = 25, sqrt(25) = 5
        REQUIRE(euclidean_distance_of_nodes(&node1, &node2) == Approx(expected_distance).epsilon(0.01));
    }

    SECTION("Test distance between identical nodes") {
        REQUIRE(euclidean_distance_of_nodes(&node1, &node1) == Approx(0.0).epsilon(0.01));
    }
}

TEST_CASE("Test calculate_distances") 
{
    Node* node1 = new Node(1, {0.0, 0.0}, {});
    Node* node2 = new Node(2, {3.0, 4.0}, {});
    Node* node3 = new Node(3, {6.0, 8.0}, {});
    
    Graph graph;
    graph.addNode(node1);
    graph.addNode(node2);
    graph.addNode(node3);
    
    vector<pair<double, int>> distances;
    calculate_distances(1, graph, distances);
    
    // node1 to node2 should be 5, node1 to node3 should be 10
    REQUIRE(distances.size() == 2); // node1 to node2 and node1 to node3
    
    // make sure distances are correct
    REQUIRE(distances[0].first == Approx(5.0).margin(0.001));
    REQUIRE(distances[0].second == 2);
    REQUIRE(distances[1].first == Approx(10.0).margin(0.001));
    REQUIRE(distances[1].second == 3);

    // clean up
    delete node1;
    delete node2;
    delete node3;
}

TEST_CASE("Test calculate_knn") 
{
    Node* node1 = new Node(1, {0.0, 0.0}, {});
    Node* node2 = new Node(2, {3.0, 4.0}, {});
    Node* node3 = new Node(3, {6.0, 8.0}, {});
    Node* node4 = new Node(4, {10.0, 10.0}, {});
    

    Graph graph;
    graph.addNode(node1);
    graph.addNode(node2);
    graph.addNode(node3);
    graph.addNode(node4);
    
    // test for k=2
    vector<pair<double, int>> knn;
    calculate_knn(1, graph, 2, knn);

    // there should be two nearest neighbors
    REQUIRE(knn.size() == 2);

    // check if the nearest neighbors are correct (node2 and node3)
    REQUIRE(knn[0].first == Approx(5.0).margin(0.001));
    REQUIRE(knn[0].second == 2);
    REQUIRE(knn[1].first == Approx(10.0).margin(0.001));
    REQUIRE(knn[1].second == 3);
    
    // test for k greater than the number of nodes
    knn.clear();
    calculate_knn(1, graph, 5, knn);
    REQUIRE(knn.size() == 3);  // since there are only 3 other nodes

    // clean up
    delete node1;
    delete node2;
    delete node3;
    delete node4;
}

