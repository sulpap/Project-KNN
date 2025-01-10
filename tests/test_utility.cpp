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

    SECTION("Test distance between (0,0,0) and (3,4,0)")
    {
        double expected_distance = 5.0;                                                           // 3^2 + 4^2 = 25, sqrt(25) = 5
        REQUIRE(euclidean_distance(coords1, coords2) == Approx(expected_distance).epsilon(0.01)); // epsilon = small range, high precision
    }

    SECTION("Test distance between identical points")
    {
        REQUIRE(euclidean_distance(coords1, coords1) == Approx(0.0).epsilon(0.01));
    }

    SECTION("Test distance with negative coordinates")
    {
        vector<double> coords3 = {-1.0, -1.0, -1.0};
        vector<double> coords4 = {2.0, 3.0, -1.0};
        double expected_distance = sqrt(pow(3.0, 2) + pow(4.0, 2) + pow(0.0, 2)); // sqrt(25) = 5
        REQUIRE(euclidean_distance(coords3, coords4) == Approx(expected_distance).epsilon(0.01));
    }

    SECTION("Test distance with large values")
    {
        vector<double> coords7 = {1e6, 1e6, 1e6};
        vector<double> coords8 = {2e6, 2e6, 2e6};
        double expected_distance = sqrt(pow(1e6, 2) + pow(1e6, 2) + pow(1e6, 2)); // sqrt(3e12) = 1e6 * sqrt(3)
        REQUIRE(euclidean_distance(coords7, coords8) == Approx(expected_distance).epsilon(0.01));
    }
}

TEST_CASE("Test euclidean_distance_floats")
{
    vector<float> coords1 = {0.0, 0.0, 0.0};
    vector<float> coords2 = {3.0, 4.0, 0.0};

    SECTION("Test distance between (0,0,0) and (3,4,0)")
    {
        double expected_distance = 5.0; // 3^2 + 4^2 = 25, sqrt(25) = 5
        REQUIRE(euclidean_distance_floats(coords1, coords2) == expected_distance);
    }

    SECTION("Test distance between identical points")
    {
        REQUIRE(euclidean_distance_floats(coords1, coords1) == 0.0);
    }

    SECTION("Test distance with negative coordinates")
    {
        vector<float> coords3 = {-1.0, -1.0, -1.0};
        vector<float> coords4 = {2.0, 3.0, -1.0};
        double expected_distance = sqrt(pow(3.0, 2) + pow(4.0, 2) + pow(0.0, 2)); // sqrt(25) = 5
        REQUIRE(euclidean_distance_floats(coords3, coords4) == expected_distance);
    }

    SECTION("Test distance with large values")
    {
        vector<float> coords7 = {1e6, 1e6, 1e6};
        vector<float> coords8 = {2e6, 2e6, 2e6};
        double expected_distance = sqrt(pow(1e6, 2) + pow(1e6, 2) + pow(1e6, 2)); // sqrt(3e12) = 1e6 * sqrt(3)
        REQUIRE(euclidean_distance_floats(coords7, coords8) == expected_distance);
    }
}

TEST_CASE("Test euclidean_distance_of_nodes")
{
    // empty edge lists
    list<Node *> empty_edges;

    Node node1(1, {0.0, 0.0, 0.0}, empty_edges, {});
    Node node2(2, {3.0, 4.0, 0.0}, empty_edges, {});

    SECTION("Test distance between two nodes (0,0,0) and (3,4,0)")
    {
        double expected_distance = 5.0; // 3^2 + 4^2 = 25, sqrt(25) = 5
        REQUIRE(euclidean_distance_of_nodes(&node1, &node2) == Approx(expected_distance).epsilon(0.01));
    }

    SECTION("Test distance between identical nodes")
    {
        REQUIRE(euclidean_distance_of_nodes(&node1, &node1) == Approx(0.0).epsilon(0.01));
    }
}

TEST_CASE("Test findMedoid")
{
    SECTION("Basic input")
    {
        vector<vector<double>> coords = {
            {1.0, 1.0},
            {2.0, 2.0},
            {3.0, 3.0}
        };

        // medoid is the point closest to all others, in this case: {2.0, 2.0} which is element 1
        int medoidIndex = findMedoid(coords);
        REQUIRE(medoidIndex == 1);
    }

    SECTION("Empty input")
    {
        vector<vector<double>> coords = {};
        int medoidIndex = findMedoid(coords);
        REQUIRE(medoidIndex == -1); // remains unchanged -> no valid medoid
    }

    SECTION("Single point")
    {
        vector<vector<double>> coords = {{1.0, 1.0}};
        int medoidIndex = findMedoid(coords);
        REQUIRE(medoidIndex == 0); // the only point is the medoid
    }

    SECTION("Identical points")
    {
        vector<vector<double>> coords = {
            {1.0, 1.0},
            {1.0, 1.0},
            {1.0, 1.0}
        };
        int medoidIndex = findMedoid(coords);
        REQUIRE(medoidIndex == 0); // any index is valid, but 0 is expected
    }
}

TEST_CASE("Test convert_to_double")
{
    vector<vector<float>> float_vector;
    vector<float> one_f;
    one_f.push_back(1.4);
    float_vector.push_back(one_f);

    vector<vector<double>> double_vector = convert_to_double(float_vector);
    REQUIRE(double_vector.size() == float_vector.size());
    vector<double> one_d = double_vector[0];
    REQUIRE(one_d.size() == one_f.size());
    REQUIRE(one_d[0] == one_f[0]);
}

TEST_CASE("initialize_graph function test")
{
    vector<vector<double>> coords = {
        {0, 1.0, 2.0},
        {1, 2.0, 3.0},
        {2, 3.0, 4.0},
    };

    Graph G;
    initialize_graph(G, coords);

    // check that the graph is not empty
    REQUIRE_FALSE(G.isEmpty());

    // check that the graph contains the correct number of nodes
    REQUIRE(G.getNodeCount() == static_cast<int>(coords.size()));

    // check that each node has the expected label and coordinates
    for (size_t i = 0; i < coords.size(); ++i)
    {
        Node *node = G.getNode(static_cast<int>(i));
        REQUIRE(node != nullptr);
        REQUIRE(node->getLabel() == static_cast<int>(coords[i][0]));

        vector<double> expectedCoords(coords[i].begin() + 1, coords[i].end());
        REQUIRE(node->getCoordinates() == expectedCoords);
    }

    G.clear();
}

TEST_CASE("computeFx function test in filteredVamana")
{
    vector<vector<double>> coords = {
        {0, 1.0, 2.0},
        {1, 2.0, 3.0},
        {0, 1.5, 2.5},
        {2, 5.0, 5.0}
    };
    // set<int> labels = {0, 1, 2};
    Graph G;
    initialize_graph(G, coords);
    unordered_map<int, set<int>> Fx = computeFx(G);

    // check that Fx contains the correct number of entries
    REQUIRE(static_cast<int>(Fx.size()) == G.getNodeCount());

    // check that each node has the correct label in Fx
    for (const auto &[nodeId, nodePtr] : G.getAdjList())
    {
        REQUIRE(Fx.find(nodeId) != Fx.end());
        int label = nodePtr->getLabel();
        REQUIRE(Fx[nodeId] == set<int>{label});
    }

    G.clear();
}