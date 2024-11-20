#include "catch.hpp"
#include "../include/bin_read.hpp"

#include <fstream>          // due to ofs
#include <cstdio>           // due to remove
#include <iostream>
using namespace std;

TEST_CASE("Test databin_read with small file") {
// Ορίζουμε τα σημεία στην αρχική μορφή τους: C | T | d1 | ... | d100
    vector<vector<float>> points_input;
    vector<float> point1 = {0, 0.0238129, 2.15454, -3.02897, 0.698244, -0.415859, 2.10826, -0.423406, -0.265021, 1.48845, -0.635659, -0.322034, 0.288786, 1.50423, 0.60534, -0.0444409, 1.72059, 0.847015, -0.307805, 0.759683, 1.29945, -0.181562, 0.463562, -0.193746, 0.649952, -1.15263, 0.048541, -0.627288, 0.398051, 0.0511783, 0.545563, 0.0495269, 0.0687726, -0.631042, 0.0826486, 0.731219, -0.226549, 0.215328, -0.148697, 0.413595, 0.0921035, 0.458715, -0.253046, -0.0336558, -0.337412, -0.170269, 0.313249, 0.498627, 0.464264, 0.672622, 0.725523, 0.003511, -0.345974, 0.498223, -0.345793, 0.161103, -0.00469103, 0.0290951, -0.28487, -0.35776, -0.37224, -0.371342, 0.515504, 0.162517, 0.0253236, 0.560636, 0.49534, 0.166332, -0.346384, 0.416897, 0.250396, -0.552659, 0.50076, 0.157336, -0.0468045, -0.460763, 0.540153, 0.0880996, -0.259423, -0.285234, 0.442767, 0.17183, 0.0573628, -0.44357, -0.247626, 0.135093, 0.238867, 0.00215004, -0.139578, -0.425506, 0.164526, 0.342528, 0.432775, -0.433014, -0.301049, -0.487173, 0.0114285, 0.386267, 0.16443, 0.0441171, 0.269416, -0.487473};
    vector<float> point2 = {5, 0.0178948, 3.26202, 1.56577, -0.0684099, -0.737793, -1.8544, -0.254527, 1.65552, 1.00833, 0.409859, -0.58229, 0.214514, 1.13399, 0.67804, 0.0556019, -0.308498, 1.36047, 0.406855, 0.0017132, 0.124005, -0.68474, -0.572021, -1.49608, -1.86856, 1.46811, 0.364149, 0.124949, -0.358147, 0.563753, 0.592939, -0.512148, -0.464038, 0.508057, -0.0594044, -0.777964, -0.14287, -0.595914, -0.351113, 0.426441, 0.00503074, 0.922792, 0.5508, 0.245417, 0.0870943, 0.287218, -0.259367, -0.0862109, 0.412294, -0.527403, -0.212971, 0.613519, -0.197322, 0.0963755, 0.191313, -0.0708244, 0.501856, -0.27319, -0.350463, -0.734249, -0.0255927, -0.558735, -0.321838, -0.0775046, 0.515006, -0.41098, -0.459845, -0.396148, -0.387778, -0.602314, -0.930792, 0.552028, 0.0922078, -0.394001, 0.445335, 0.303109, 0.495165, -0.145585, 0.0042617, -0.00186817, -0.0232262, 0.36179, 0.322431, 0.340549, 0.697516, -0.00626198, 0.324457, 0.806469, 0.302298, 0.282206, -0.203537, -0.131816, 0.0834625, -0.182271, 0.0991221, 0.359114, -0.522822, -0.523069, -0.486342, 0.0311416, -0.123481, -0.303604};
    points_input.push_back(point1);
    points_input.push_back(point2);

// Γράφουμε τα points σε binary αρχείο, στη μορφή που η databin_read περιμένει να διαβάσει το αρχείο.
// Προσθέτουμε, δηλαδή, στην αρχή του αρχείου έναν ακέραιο που υποδεικνύει το πλήθος των points που περιέχει το αρχείο
// source: https://github.com/transactionalblog/sigmod-contest-2024/blob/eeba09101429f2fffe18e02a282b4bb70b87ddd1/io.h#L22
    ofstream ofs("output.bin", ios::out | ios::binary);
    const int num_dimensions = 102;
    const uint32_t N = points_input.size();

    // Save value of N first into the file
    ofs.write(reinterpret_cast<const char *>(&N), sizeof(uint32_t));        // N is an int

    assert(points_input.front().size() == num_dimensions);
    for (unsigned i = 0; i < N; ++i) {
        auto const &point = points_input[i];
        ofs.write(reinterpret_cast<char const *>(&point[0]), num_dimensions * sizeof(float));
    }
    ofs.close();

// Τα αποτελέσματα της databin_read θα πρέπει να είναι τα εξής:
// (στα points δεν θα πρέπει να υπάρχει το πεδίο Τ, αλλά τα υπόλοιπα να είναι ίδια)
    vector<vector<float>> points_output;
    vector<float> p1 = {0, 2.15454, -3.02897, 0.698244, -0.415859, 2.10826, -0.423406, -0.265021, 1.48845, -0.635659, -0.322034, 0.288786, 1.50423, 0.60534, -0.0444409, 1.72059, 0.847015, -0.307805, 0.759683, 1.29945, -0.181562, 0.463562, -0.193746, 0.649952, -1.15263, 0.048541, -0.627288, 0.398051, 0.0511783, 0.545563, 0.0495269, 0.0687726, -0.631042, 0.0826486, 0.731219, -0.226549, 0.215328, -0.148697, 0.413595, 0.0921035, 0.458715, -0.253046, -0.0336558, -0.337412, -0.170269, 0.313249, 0.498627, 0.464264, 0.672622, 0.725523, 0.003511, -0.345974, 0.498223, -0.345793, 0.161103, -0.00469103, 0.0290951, -0.28487, -0.35776, -0.37224, -0.371342, 0.515504, 0.162517, 0.0253236, 0.560636, 0.49534, 0.166332, -0.346384, 0.416897, 0.250396, -0.552659, 0.50076, 0.157336, -0.0468045, -0.460763, 0.540153, 0.0880996, -0.259423, -0.285234, 0.442767, 0.17183, 0.0573628, -0.44357, -0.247626, 0.135093, 0.238867, 0.00215004, -0.139578, -0.425506, 0.164526, 0.342528, 0.432775, -0.433014, -0.301049, -0.487173, 0.0114285, 0.386267, 0.16443, 0.0441171, 0.269416, -0.487473};
    vector<float> p2 = {5, 3.26202, 1.56577, -0.0684099, -0.737793, -1.8544, -0.254527, 1.65552, 1.00833, 0.409859, -0.58229, 0.214514, 1.13399, 0.67804, 0.0556019, -0.308498, 1.36047, 0.406855, 0.0017132, 0.124005, -0.68474, -0.572021, -1.49608, -1.86856, 1.46811, 0.364149, 0.124949, -0.358147, 0.563753, 0.592939, -0.512148, -0.464038, 0.508057, -0.0594044, -0.777964, -0.14287, -0.595914, -0.351113, 0.426441, 0.00503074, 0.922792, 0.5508, 0.245417, 0.0870943, 0.287218, -0.259367, -0.0862109, 0.412294, -0.527403, -0.212971, 0.613519, -0.197322, 0.0963755, 0.191313, -0.0708244, 0.501856, -0.27319, -0.350463, -0.734249, -0.0255927, -0.558735, -0.321838, -0.0775046, 0.515006, -0.41098, -0.459845, -0.396148, -0.387778, -0.602314, -0.930792, 0.552028, 0.0922078, -0.394001, 0.445335, 0.303109, 0.495165, -0.145585, 0.0042617, -0.00186817, -0.0232262, 0.36179, 0.322431, 0.340549, 0.697516, -0.00626198, 0.324457, 0.806469, 0.302298, 0.282206, -0.203537, -0.131816, 0.0834625, -0.182271, 0.0991221, 0.359114, -0.522822, -0.523069, -0.486342, 0.0311416, -0.123481, -0.303604};
    points_output.push_back(p1);
    points_output.push_back(p2);

// Κλήση συνάρτησης & έλεγχος αποτελεσμάτων
    vector<vector<float>> points_results = databin_read("output.bin");
    REQUIRE(points_output == points_results);

// Διαγραφή του αρχείου "output.bin"
    remove("output.bin");
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