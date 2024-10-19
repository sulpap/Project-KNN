#include "catch.hpp"

#include "../include/graph.hpp"

using namespace std;

TEST_CASE("Creating a Node, no Edges", "[new_node]") {
    int id = 1;
    vector<double> coordinates;
    coordinates.push_back(10.0);
    coordinates.push_back(20.0);
    list<Node*> edges;
    Node* testing_node = new Node(id, coordinates, edges);

    REQUIRE(testing_node->getId() == 1);
    REQUIRE(testing_node->getGraphId() == 0);
    REQUIRE(testing_node->getCoordinates().at(0) == 10.0);
    REQUIRE(testing_node->getCoordinates().at(1) == 20.0);
    REQUIRE(testing_node->getEdges().size() == 0);
}