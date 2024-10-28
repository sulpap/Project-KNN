#include "catch.hpp"
#include "../include/fvecs_read.hpp"
#include <fstream>

using namespace std;

// helper function - creates an fvecs file
void create_fvecs(const char *filename)
{
    // TODO:
    // put 2 vextors: 
    // {1.0, 2.0, 3.0}
    // {4.0, 5.0, 6.0}

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
        // TODO
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
        // TODO

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

    // cleanup
    remove(test_file);
} 

