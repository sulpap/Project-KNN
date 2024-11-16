#include "../include/bin_read.hpp"

#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

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