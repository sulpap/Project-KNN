#ifndef BIN_READ_HPP
#define BIN_READ_HPP

#include <vector>
using namespace std;

/* Συνάρτηση που διαβάζει ένα data.bin αρχείο (με τη δομή που περιγράφεται
στο https://transactional.blog/sigmod-contest/2024) και επιστρέφει
vector <vector<float>> points, όπου το κάθε vector στο 0-στο στοιχείο έχει
την τιμή του C (>=0), ενώ από το 1ο στοιχείο μέχρι και το 101 στοιχείο είναι
οι συντεταγμένες του σημείου */
vector<vector<float>> databin_read(const char *filename);

/* Συνάρτηση που διαβάζει ένα queries.bin αρχείο (με τη δομή που περιγράφεται
στο https://transactional.blog/sigmod-contest/2024)
και επιστρέφει vector <vector<float>> queries,
όπου το κάθε vector στο 0-στο στοιχείο έχει τιμή 0 ή 1 (query type),
στο 1ο στοιχείο έχει τιμή του C (>=0 αν έχει φίλτρο ή -1 αν δεν έχει φίλτρο),
ενώ από το 2ο στοιχείο μέχρι και το 102 στοιχείο είναι οι συντεταγμένες του query */
vector<vector<float>> queriesbin_read(const char *filename);

// * Following function will be used only for brute force ground truth
// If query_type == 1, and point doesn't match the filter of the query,
// then we return -1 as dist
double eucl_dist_point_query(vector<float> &point, vector<float> &query);

/* Συνάρτηση που παίρνει ως παράμετρο ένα vector<vector<float>> queries
με queries που έχουν query_type 0 ή 1 (+ τιμή του C + dimensions query) και επιστρέφει το ground truth
για κάθε ένα από αυτά. Τα points που δίνονται ως παράμετρο *δεν* περιέχουν timestamp.

The groundtruth files contain, for each query, the identifiers (vector number, starting at 0)
of its k nearest neighbors, ordered by *increasing* (squared euclidean) distance. */
vector<vector<int>> ground_truth(vector<vector<float>> &points, vector<vector<float>> &queries, int k);

/* Συνάρτηση που παίρνει ως παράμετρο ένα binary ground truth file το οποίο παρήχθη από το src/calculate_groundtruth.cpp
με τη δομή που περιγράφεται στο src/calculate_groundtruth.cpp και την τιμή του k για το οποίο παρήχθη το ground truth file.
Επιστρέφει το binary ground truth file σε vector της μορφής: vector<vector<int>> */
vector<vector<int>> gtbin_read(const char *filename, int k);

#endif