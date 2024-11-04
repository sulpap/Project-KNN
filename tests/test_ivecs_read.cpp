#include "catch.hpp"
#include "../include/ivecs_read.hpp"
#include <fstream>

using namespace std;

// Helper function - creates an ivecs file
void create_ivecs(const char *filename)
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
        int vec1[] = {1, 2, 3};
        file.write(reinterpret_cast<const char *>(vec1), dim * sizeof(int));

        // Write the second vector {4, 5, 6}
        file.write(reinterpret_cast<const char *>(&dim), sizeof(dim));
        int vec2[] = {4, 5, 6};
        file.write(reinterpret_cast<const char *>(vec2), dim * sizeof(int));

        file.close();
    }
}

TEST_CASE("Test ivecs_read with small file")
{
    const char *test_file = "test_ivecs.ivecs";
    create_ivecs(test_file);

    SECTION("Read all vectors with default parameters")
    {
        vector<vector<int>> vectors = ivecs_read(test_file);

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
        vector<vector<int>> vectors = ivecs_read(test_file, a, b);

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

    SECTION("Test ivecs_read with an out-of-range query")
    {
        size_t a = 3, b = 5;
        vector<vector<int>> vectors = ivecs_read(test_file, a, b);

        REQUIRE(vectors.empty());
    }

    remove(test_file);
}
