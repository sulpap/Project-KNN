#include "catch.hpp"
#include "../include/utility.hpp"
#include <vector>

using namespace std;

TEST_CASE("Test findMedoid") 
{
    vector<vector<double>> coords = {
        {1.0, 1.0}, 
        {2.0, 2.0}, 
        {3.0, 3.0}
    };

    // medoid is the point closest to all others, in this case: {2.0, 2.0} which is element 1
    int medoidIndex = findMedoid(coords);
    cout << "Medoid Index for Test 1: " << medoidIndex << endl;
    REQUIRE(medoidIndex == 1);
} 

// TODO are there edge cases?????