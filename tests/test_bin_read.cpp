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

TEST_CASE("Test ground_truth with small amount of points, queries") {
// [1] Δημιουργία των points
    vector<vector<float>> points;

    vector<float> p0;
    p0.push_back(2);        // label of p0
    p0.push_back(1.0);      // d1 of p0
    p0.push_back(1.0);      // d2 of p0
    points.push_back(p0);

    vector<float> p1;
    p1.push_back(2);        // label of p1
    p1.push_back(4.0);      // d1 of p1
    p1.push_back(4.0);      // d2 of p1
    points.push_back(p1);

    vector<float> p2;
    p2.push_back(2);        // label of p2
    p2.push_back(5.0);      // d1 of p2
    p2.push_back(3.0);      // d2 of p2
    points.push_back(p2);

    vector<float> p3;
    p3.push_back(3);        // label of p3
    p3.push_back(6.0);      // d1 of p3
    p3.push_back(2.0);      // d2 of p3
    points.push_back(p3);

// [2] Δημιουργία των queries
    vector<vector<float>> queries;

    vector<float> q0;
    q0.push_back(1);        // query_type of q0
    q0.push_back(2);        // label of q0
    q0.push_back(3.0);      // d1 of q0
    q0.push_back(3.0);      // d2 of q0
    queries.push_back(q0);

    vector<float> q1;
    q1.push_back(1);        // query_type of q1
    q1.push_back(3);        // label of q1
    q1.push_back(3.0);      // d1 of q1
    q1.push_back(3.0);      // d2 of q1
    queries.push_back(q1);

    vector<float> q2;
    q2.push_back(0);        // query_type of q2
    q2.push_back(-1);       // label of q2
    q2.push_back(3.0);      // d1 of q2
    q2.push_back(3.0);      // d2 of q2
    queries.push_back(q2);

// [3] Εύρεση ground truth για κάθε query με k = 2
    vector<vector<int>> gt_sol;

    vector<int> gt_q0;      // ground truth q0
    gt_q0.push_back(1);     // point με id = 1
    gt_q0.push_back(2);     // point με id = 2
    gt_sol.push_back(gt_q0);

    vector<int> gt_q1;      // ground truth q1
    gt_q1.push_back(3);     // point με id = 3
    gt_sol.push_back(gt_q1);

    gt_sol.push_back(gt_q0);    // ground truth q2 ταυτίζεται με ground truth q0


// [4] Επιβέβαιωση gt_sol με συνάρτησης
    int k = 2;
    vector<vector<int>> gt_func = ground_truth(points, queries, k);

    REQUIRE(gt_sol == gt_func);
}