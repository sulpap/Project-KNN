#ifndef BIN_READ_HPP
#define BIN_READ_HPP

#include <vector>
using  namespace std;

/* Συνάρτηση που διαβάζει ένα data.bin αρχείο και επιστρέφει
vector <vector<float>> points, όπου το κάθε vector στο 0-στο στοιχείο έχει
την τιμή του C (>=0), ενώ από το 1ο στοιχείο μέχρι και το 101 στοιχείο είναι
οι συντεταγμένες του σημείου */
vector<vector<float>> databin_read(const char* filename);

/* Συνάρτηση που διαβάζει ένα queries.bin αρχείο και επιστρέφει vector <vector<float>> queries,
όπου το κάθε vector στο 0-στο στοιχείο έχει τιμή 0 ή 1 (query type),
στο 1ο στοιχείο έχει τιμή του C (>=0 αν έχει φίλτρο ή -1 αν δεν έχει φίλτρο),
ενώ από το 2ο στοιχείο μέχρι και το 102 στοιχείο είναι οι συντεταγμένες του query */
vector<vector<float>> queriesbin_read(const char* filename);

// * Following function will be used only for brute force ground truth
// If query_type == 1, and point doesn't match the filter of the query,
// then we return -1 as dist
double eucl_dist_point_query(vector<float> &point, vector<float> &query);

#endif