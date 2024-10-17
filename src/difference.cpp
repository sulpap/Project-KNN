#include <vector>
#include <set>
#include <algorithm>            // due to set_difference
#include <iostream>             // due to cout
using namespace std;

struct Point {                  // by default everything is public
    int id; 
    vector<double> coordinates;

    // The overloaded version of < will be used, only when comparing objects of struct Point
    bool operator<(const Point& other) const {
        return id < other.id;
    }
};

set<Point> my_difference(const set<Point>& set1, const set<Point>& set2) {      // parameters are passed by reference!
    // Initialize a set to store the difference
    set<Point> difference;

    // Use set_difference from library <algorithm> to find the difference
    set_difference(
        set1.begin(), set1.end(), set2.begin(), set2.end(),
        inserter(difference, difference.begin()));

    return difference;          // we return by value
}


int main(void) {
    struct Point point_1;
    point_1.id = 5;
    point_1.coordinates.push_back(1.2);
    point_1.coordinates.push_back(3.4);

    struct Point point_2;
    point_2.id = 8;
    point_2.coordinates.push_back(4.4);
    point_2.coordinates.push_back(5.8);

    set<Point> set1 = { point_1 };
    set<Point> set2 = { point_2 };
    // set<Point> set2 = { point_2, point_1 };


    // Initialize a set to store the difference
    set<Point> difference;
    difference = my_difference(set1, set2);

    for (const auto& elem : difference) {       // auto: allows the compiler to automatically deduce the type of elem based on the type of elements in difference
        cout << elem.id << endl;
    }

    return 0;
}


// TODO: Function that creates a set given a graph