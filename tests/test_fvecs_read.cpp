#include "catch.hpp"
#include "../include/fvecs_read.hpp"
#include <fstream>

using namespace std;

// helper function - creates an fvecs file
void create_fvecs(const char *filename)
{
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

TEST_CASE("Test fvecs_read with small file") 
{
    // create an fvecs file for testing.
    const char *test_file = "test.fvecs";
    create_fvecs(test_file);

    SECTION("Read all vectors with default parameters")
    {
        vector<vector<float>> vectors = fvecs_read(test_file);

        // check that there are 2 vectors.
        REQUIRE(vectors.size() == 2);

        // check the dimensions of the vectors.
        REQUIRE(vectors[0].size() == 3);
        REQUIRE(vectors[1].size() == 3);


        // check the contents of the vectors.

        // first vector:
        REQUIRE(vectors[0][0] == Approx(1.0).margin(0.001));
        REQUIRE(vectors[0][1] == Approx(2.0).margin(0.001));
        REQUIRE(vectors[0][2] == Approx(3.0).margin(0.001));

        // second vector:
        REQUIRE(vectors[1][0] == Approx(4.0).margin(0.001));
        REQUIRE(vectors[1][1] == Approx(5.0).margin(0.001));
        REQUIRE(vectors[1][2] == Approx(6.0).margin(0.001));

    }

    SECTION("Read all vectors with specific parameters")
    {
        size_t a, b;
        a = 1;
        b = 2;
        vector<vector<float>> vectors = fvecs_read(test_file, a, b);

        // check that there are 2 vectors.
        REQUIRE(vectors.size() == 2);

        // check the dimensions of the vectors.
        REQUIRE(vectors[0].size() == 3);
        REQUIRE(vectors[1].size() == 3);

        // check the contents of the vectors.
        REQUIRE(vectors[0][0] == Approx(1.0).margin(0.001));
        REQUIRE(vectors[0][1] == Approx(2.0).margin(0.001));
        REQUIRE(vectors[0][2] == Approx(3.0).margin(0.001));

        REQUIRE(vectors[1][0] == Approx(4.0).margin(0.001));
        REQUIRE(vectors[1][1] == Approx(5.0).margin(0.001));
        REQUIRE(vectors[1][2] == Approx(6.0).margin(0.001));

    }
    
    SECTION("Test fvecs_read with an out-of-range query") 
    {
        // there are no vectors in [3,5]
        size_t a, b;
        a = 3;
        b = 5;
        vector<vector<float>> vectors = fvecs_read(test_file, a, b);

        // we expect an empty vector as a result.
        REQUIRE(vectors.empty());
    }

    // cleaning
    remove(test_file);
} 

