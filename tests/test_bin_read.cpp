#include "catch.hpp"
#include "../include/bin_read.hpp"

#include <iostream>
using namespace std;

TEST_CASE("Test databin_read with small file") {
    // TODO
}

TEST_CASE("Test queriesbin_read_read with small file") {
    // TODO
}

TEST_CASE("Test eucl_dist_point_query") {
// Case [1]: query_type = 1, and point matches filter of query
    vector<float> point1 = {1, 0.0, 0.0, 0.0};
    vector<float> query1 = {1, 1, 3.0, 4.0, 0.0};

    double expected_distance = 5.0;                                        // 3^2 + 4^2 = 25, sqrt(25) = 5
    REQUIRE(eucl_dist_point_query(point1, query1) == expected_distance); 

// Case [2]: query_type = 1, and point doesn't match filter of query
    vector<float> point2 = {1, 0.0, 0.0, 0.0};
    vector<float> query2 = {1, 5, 3.0, 4.0, 0.0};

    expected_distance = -1;                                                           
    REQUIRE(eucl_dist_point_query(point2, query2) == expected_distance); 

// Case [3]: query_type = 0
    vector<float> point3 = {5, 0.0, 0.0, 0.0};
    vector<float> query3 = {0, -1, 3.0, 4.0, 0.0};

    expected_distance = 5.0;                                                           
    REQUIRE(eucl_dist_point_query(point3, query3) == expected_distance); 
}