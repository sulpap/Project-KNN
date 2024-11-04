#include "catch.hpp"
#include "../include/bvecs_read.hpp"
#include <fstream>

using namespace std;

// Helper function - creates a bvecs file
void create_bvecs(const char *filename)
{
    // Put 2 vectors:
    // {1, 2, 3}
    // {4, 5, 6}

    ofstream file(filename, ios::binary);
    if (file.is_open())
    {
        int dim = 3;

        // Write the first vector {1, 2, 3}
        file.write(reinterpret_cast<const char *>(&dim), sizeof(dim));
        uint8_t vec1[] = {1, 2, 3};
        file.write(reinterpret_cast<const char *>(vec1), dim * sizeof(uint8_t));

        // Write the second vector {4, 5, 6}
        file.write(reinterpret_cast<const char *>(&dim), sizeof(dim));
        uint8_t vec2[] = {4, 5, 6};
        file.write(reinterpret_cast<const char *>(vec2), dim * sizeof(uint8_t));

        file.close();
    }
}

TEST_CASE("Test bvecs_read with small file")
{
    const char *test_file = "test_bvecs.bvecs";
    create_bvecs(test_file);

    SECTION("Read all vectors with default parameters")
    {
        vector<vector<uint8_t>> vectors = bvecs_read(test_file);

        REQUIRE(vectors.size() == 2);
        REQUIRE(vectors[0].size() == 3);
        REQUIRE(vectors[1].size() == 3);

        REQUIRE(vectors[0][0] == 1);
        REQUIRE(vectors[0][1] == 2);
        REQUIRE(vectors[0][2] == 3);

        REQUIRE(vectors[1][0] == 4);
        REQUIRE(vectors[1][1] == 5);
        REQUIRE(vectors[1][2] == 6);
    }

    SECTION("Read all vectors with specific parameters")
    {
        size_t a = 1, b = 2;
        vector<vector<uint8_t>> vectors = bvecs_read(test_file, a, b);

        REQUIRE(vectors.size() == 2);
        REQUIRE(vectors[0].size() == 3);
        REQUIRE(vectors[1].size() == 3);

        REQUIRE(vectors[0][0] == 1);
        REQUIRE(vectors[0][1] == 2);
        REQUIRE(vectors[0][2] == 3);

        REQUIRE(vectors[1][0] == 4);
        REQUIRE(vectors[1][1] == 5);
        REQUIRE(vectors[1][2] == 6);
    }

    SECTION("Test bvecs_read with an out-of-range query")
    {
        size_t a = 3, b = 5;
        vector<vector<uint8_t>> vectors = bvecs_read(test_file, a, b);

        REQUIRE(vectors.empty());
    }

    remove(test_file);
}
