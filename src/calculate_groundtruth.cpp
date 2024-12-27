#include <stdio.h>
#include <cstdlib>          // atoi
#include <fstream>          // ofs
#include <cassert>
#include <iostream>
#include <cstdint>                  // due to uint32_t

#include <chrono>

using namespace std;

#include "../include/bin_read.hpp"

int main(int argc, char* argv[]) {
    int k = 100;            // If no argument is given, we calculate ground truth for k = 100
    if(argc == 2)
        k = atoi(argv[1]);

    printf("Hello from calculate_groundtruth with k = %d!\n", k);

    vector<vector<float>> points = databin_read("datasets/smallscale/dummy-data.bin");
    vector<vector<float>> queries = queriesbin_read("datasets/smallscale/dummy-queries.bin");
    auto start = chrono::high_resolution_clock::now();
    vector<vector<int>> gt = ground_truth(points, queries, k);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    /*
    Δομή binary ground truth:
    Δεδομένου ότι έχουμε 5012 queries, το ground truth έχει 5012 γραμμές.
    Για κάθε γραμμή, πρώτα γράφουμε το πλήθος των nearest neighbors που βρήκαμε για το query της συγκεκριμένης γραμμής 
    (καθώς υπάρχει περίπτωση να βρεθούν λιγότεροι από k γείτονες για το συγκεκριμένο query). Στη συνέχεια, γράφουμε τα ids
    των points των nearest γειτόνων του query
    */

    string output_file = "datasets/smallscale/gt_k=" + to_string(k) + ".bin";

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