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