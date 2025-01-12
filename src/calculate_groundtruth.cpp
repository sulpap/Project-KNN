#include <stdio.h>
#include <cstdlib>          // atoi
#include <fstream>          // ofs
#include <cassert>
#include <iostream>
#include <cstdint>                  // due to uint32_t

#include <chrono>

using namespace std;

#include "../include/bin_read.hpp"

/* Αν θέλουμε να το τρέξουμε για k = 100, τότε δεν δίνουμε το k ως παράμετρο.
Διαφορετικά δίνουμε το k ως 2η παράμετρο μετά το όνομα του προγράμματος.
Αν θέλουμε να το τρέξουμε για το smallscale dataset, τότε δεν χρειάζεται να δώσουμε τα points, queries.
Διαφορετικά, αν θέλουμε να το τρέξουμε για το mediumscale dataset, χρειάζεται να δώσουμε τα μονοπότια των points, queries.
IMPORTANT! Δεν το έχουμε υλοποιήσει ακόμη, ώστε να τρέχει για large scale dataset
Παραδείγματα εκτέλεσης:
./bin/calculate_groundruth              (runs for k = 100, and smallscale dataset)
./bin/calculate_groundtruth 20          (runs for k = 20, and smallscale dataset)
./bin/calculate_groundtruth datasets/mediumscale/<insert name of points>.bin datasets/mediumscale/<insert name of queries>.bin         (runs for k = 100, and mediumscale dataset -takes 8.5 minutes to run)
./bin/calculate_groundtruth 20 datasets/mediumscale/<insert name of points>.bin datasets/mediumscale/<insert name of queries>.bin      (runs for k = 20, and mediumscale dataset -takes 8.4 minutes to run)
*/
int main(int argc, char* argv[]) {
    int k = 100;            // If no argument is given, we calculate ground truth for k = 100 and for the smallscale dataset
    const char *filename_points = "datasets/smallscale/dummy-data.bin";
    const char *filename_queries = "datasets/smallscale/dummy-queries.bin";
    const char* filename_output = "datasets/smallscale/gt_k=";

    if(argc == 2 || argc == 4)
        k = atoi(argv[1]);

    if(argc > 2) {
        assert(argc == 4 || argc == 3);          // No more than 4 arguments should be given
        filename_points = argv[argc-2];
        filename_queries = argv[argc-1];
        filename_output = "datasets/mediumscale/gt_k=";
    }

    printf("Hello from calculate_groundtruth with k = %d!\n", k);

    vector<vector<float>> points = databin_read(filename_points);
    vector<vector<float>> queries = queriesbin_read(filename_queries);
    auto start = chrono::high_resolution_clock::now();
    vector<vector<int>> gt = ground_truth(points, queries, k);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    /*
    Δομή binary ground truth:
    Δεδομένου ότι έχουμε 5012 queries (!this is true for smallscate and mediumscale dataset!), το ground truth έχει 5012 γραμμές.
    Για κάθε γραμμή, πρώτα γράφουμε το πλήθος των nearest neighbors που βρήκαμε για το query της συγκεκριμένης γραμμής 
    (καθώς υπάρχει περίπτωση να βρεθούν λιγότεροι από k γείτονες για το συγκεκριμένο query). Στη συνέχεια, γράφουμε τα ids
    των points των nearest γειτόνων του query
    */

    string output_file = filename_output + to_string(k) + ".bin";

    ofstream ofs(output_file, ios::out | ios::binary);
    assert(gt.size() == 5012);
    for (unsigned i = 0; i < gt.size(); i++) {
        auto const &gt_sol = gt[i];
        int num_dimensions = gt_sol.size();

        // Save num_dimensions first into the file
        ofs.write(reinterpret_cast<const char *>(&num_dimensions), sizeof(uint32_t));        // num_dimensions is an int

        // Save ids of neighbors
        ofs.write(reinterpret_cast<char const *>(&gt_sol[0]), num_dimensions * sizeof(int));
    }
    ofs.close();

    cout << "Groundtruth file saved at: ./" << output_file << endl;
    cout << "Groundtruth calculation took: " << duration.count() << " seconds" << endl;

    return 0;
}