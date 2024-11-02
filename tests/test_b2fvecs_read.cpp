#include "catch.hpp"
#include "../include/b2fvecs_read.hpp"
#include <fstream>

using namespace std;

// helper function - creates a b2fvecs file
void create_b2fvecs(const char *filename) {
    // put 2 vectors: 
    // {1.0, 2.0, 3.0}
    // {4.0, 5.0, 6.0}

    ofstream file(filename, ios::binary);
    if (file.is_open()) {
        int dim = 3;

        // Write the first vector {1.0, 2.0, 3.0}
        file.write(reinterpret_cast<const char*>(&dim), sizeof(dim));
        float vec1[] = {1.0, 2.0, 3.0};
        file.write(reinterpret_cast<const char*>(vec1), dim * sizeof(float));

        // Write the second vector {4.0, 5.0, 6.0}
        file.write(reinterpret_cast<const char*>(&dim), sizeof(dim));
        float vec2[] = {4.0, 5.0, 6.0};
        file.write(reinterpret_cast<const char*>(vec2), dim * sizeof(float));

        file.close();
    }
}

TEST_CASE("Test b2fvecs_read with small file") {
    // create a bvecs file for testing.
    const char *test_file = "test_b2fvecs.bvecs";
    create_b2fvecs(test_file);

    SECTION("Read all vectors with default parameters") {
        vector<vector<float>> vectors = b2fvecs_read(test_file);
        REQUIRE(vectors.size() == 2);
        REQUIRE(vectors[0].size() == 3);
        REQUIRE(vectors[1].size() == 3);

        REQUIRE(vectors[0][0] == Approx(1.0).margin(0.001));
        REQUIRE(vectors[0][1] == Approx(2.0).margin(0.001));
        REQUIRE(vectors[0][2] == Approx(3.0).margin(0.001));

        REQUIRE(vectors[1][0] == Approx(4.0).margin(0.001));
        REQUIRE(vectors[1][1] == Approx(5.0).margin(0.001));
        REQUIRE(vectors[1][2] == Approx(6.0).margin(0.001));
    }

    SECTION("Read all vectors with specific parameters") {
        vector<vector<float>> vectors = b2fvecs_read(test_file, 1, 2);
        REQUIRE(vectors.size() == 2);
        REQUIRE(vectors[0].size() == 3);
        REQUIRE(vectors[1].size() == 3);

        REQUIRE(vectors[0][0] == Approx(1.0).margin(0.001));
        REQUIRE(vectors[0][1] == Approx(2.0).margin(0.001));
        REQUIRE(vectors[0][2] == Approx(3.0).margin(0.001));

        REQUIRE(vectors[1][0] == Approx(4.0).margin(0.001));
        REQUIRE(vectors[1][1] == Approx(5.0).margin(0.001));
        REQUIRE(vectors[1][2] == Approx(6.0).margin(0.001));
    }


        SECTION("Test b2fvecs_read with an out-of-range query") {

        vector<vector<float>> vectors = b2fvecs_read(test_file, 3, 5);
        REQUIRE(vectors.empty());
    }
    

    remove(test_file);
}