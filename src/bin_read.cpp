#include "../include/bin_read.hpp"
#include "../include/utility.hpp"
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <algorithm>                // sort

#include "assert.h"
using namespace std;

// Source: https://github.com/transactionalblog/sigmod-contest-2024/blob/eeba09101429f2fffe18e02a282b4bb70b87ddd1/io.h#L40
static void ReadBin(const string &file_path, const int num_dimensions, vector<vector<float>> &data) {
    ifstream ifs;
    ifs.open(file_path, ios::binary);
    assert(ifs.is_open());

    uint32_t N; // num of points
    ifs.read((char *)&N, sizeof(uint32_t));
    data.resize(N);

    vector<float> buff(num_dimensions);
    int counter = 0;

    while (ifs.read((char *)buff.data(), num_dimensions * sizeof(float))) {
        vector<float> row(num_dimensions);
        for (int d = 0; d < num_dimensions; d++) {
            row[d] = static_cast<float>(buff[d]);
        }
        data[counter++] = move(row);
    }

    ifs.close();
}

/* Συνάρτηση που διαβάζει ένα data.bin αρχείο και επιστρέφει 
vector <vector<float>> points, όπου το κάθε vector στο 0-στο στοιχείο έχει
την τιμή του C (>=0), ενώ από το 1ο στοιχείο μέχρι και το 101 στοιχείο είναι 
οι συντεταγμένες του σημείου */
vector<vector<float>> databin_read(const char* filename) {
    vector<vector<float>> points;
    ReadBin(filename, 102, points);     // A vector point is: C | T | d1 | ... | d100 |

    // We change a vector point to: C | d1 | ... | d100
    vector<vector<float>> points_better(points.size(), vector<float>(101));
    for (int i = 0; i < static_cast<int>(points.size()); i++) {
        int k = 0;
        for (int j = 0; j < static_cast<int>(points[i].size()); j++) {
            if (j == 1) { // Skip the second dimension (T value)
                continue;
            }
            points_better[i][k] = points[i][j]; // Copy values into points_better, skipping index 1 which contains T
            k++;
        }
    }
    return points_better;
}

/* Συνάρτηση που διαβάζει ένα queries.bin αρχείο και επιστρέφει vector <vector<float>> queries, 
όπου το κάθε vector στο 0-στο στοιχείο έχει τιμή 0 ή 1 (query type), 
στο 1ο στοιχείο έχει τιμή του C (>=0 αν έχει φίλτρο ή -1 αν δεν έχει φίλτρο), 
ενώ από το 2ο στοιχείο μέχρι και το 102 στοιχείο είναι οι συντεταγμένες του query */
vector<vector<float>> queriesbin_read(const char* filename) {
    vector<vector<float>> queries;
    ReadBin(filename, 104, queries);     // A vector query is: qt | v | l | r | d1 | ... | d100 |

    vector<vector<float>> queries_better;
    for (int i = 0; i < static_cast<int>(queries.size()); i++) {
        if (queries[i][0] == 0 || queries[i][0] == 1) {         // Επιστρέφουμε queries μόνο με query type = 0 ή 1
            vector<float> vec;
            // We change a vector query to: qt | v | d1 | ... | d100
            // Add the first two values (query_type and v)
            vec.push_back(queries[i][0]); // qt
            vec.push_back(queries[i][1]); // v

            // Add the remaining dimensions, skipping indices 2 and 3
            for (int j = 4; j < static_cast<int>(queries[i].size()); j++) {
                vec.push_back(queries[i][j]);
            }
            queries_better.push_back(vec);
        }
    }
    return queries_better;
}


// * Following function will be used only for brute force ground truth
// If query_type == 1, and point doesn't match the filter of the query,
// then we return -1 as dist
double eucl_dist_point_query(vector<float> &point, vector<float> &query) {
    if(query[0] == 1 && query[1] != point[0]) {
        return -1;
    }

    vector<float> point_coord(point.begin() + 1, point.end());      // We take from point only its dimensions
    vector<float> query_coord(query.begin() + 2, query.end());      // We take from query only its dimensions

    assert(point_coord.size() == query_coord.size());   // number of actual dimensions to compare should be the same

    return euclidean_distance_floats(point_coord, query_coord);        // return dist
}


/* Συνάρτηση που παίρνει ως παράμετρο ένα vector<vector<float>> queries
με queries που έχουν query_type 0 ή 1 και επιστρέφει το ground truth
για κάθε ένα από αυτά 

The groundtruth files contain, for each query, the identifiers (vector number, starting at 0) 
of its k nearest neighbors, ordered by *increasing* (squared euclidean) distance. */
vector<vector<int>> ground_truth(vector<vector<float>> &points, vector<vector<float>> &queries, int k) {
    vector<vector<int>> knn_results;

    for(u_int i = 0; i < queries.size(); i++) {       // Για κάθε query
        int query_type = queries[i][0];
        assert(query_type == 0 || query_type == 1); // Handling 2 types of queries only

        vector<double> dists;
        dists.resize(points.size());
        for(u_int j = 0; j < points.size(); j++) {
            dists[j] = eucl_dist_point_query(points[j], queries[i]);
            // if query_type == 1, and point doesn't match the filter of the query,
            // then above function will return -1 as dist
        }

        vector<int> ids;
        ids.resize(points.size());
        iota(ids.begin(), ids.end(), 0);

        // Sort ids based on dists
        sort(ids.begin(), ids.end(), [&](int a, int b){
            return dists[a] < dists[b];
        });

        vector<int> knn_sorted;
        // About to fill up knn_sorted:
        for(u_int j = 0; j < points.size(); j++) {
            if(dists[ids[j]] == -1) {           // We have a query with query_type = 1, and current point doesn't match filter of query
                continue;                       // Therefore, we skip the distance between the two
            }
            knn_sorted.push_back(ids[j]);
            if(static_cast<int>(knn_sorted.size()) >= k) 
                break;
        }
        // Save ground truth of current query
        knn_results.push_back(knn_sorted);
    }
    return knn_results;
}