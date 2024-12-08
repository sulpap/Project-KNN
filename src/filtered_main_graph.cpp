#include "../include/bin_read.hpp"
#include "../include/graph.hpp"
#include "../include/utility.hpp"
#include "../include/filteredVamana.hpp"
#include "../include/filteredGreedySearch.hpp"
#include "../include/graph_binary_io.hpp"
#include <chrono>
#include <algorithm>    // find
#include <cassert>
#include <set>
#include <cstdlib>      
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
    if (argc != 8) 
    {
        cout << "Usage: " << argv[0] << " <k> <L> <R> <a> <t> <base_file_path> <groundtruth_file_path> " << endl;
        cout << "Note: k must be an int\n";
        cout << "      L must be an int\n";
        cout << "      R must be an int\n";
        cout << "      a must be a double\n";
        cout << "      t must be an int" << endl;
        return 1;
    }

    int k = atoi(argv[1]);
    int L = atoi(argv[2]);
    int R = atoi(argv[3]);
    double a = stod(argv[4]);
    int taph = atoi(argv[5]);

    cout << "\nFilteredVamana procedure that saves the graph starting..." << endl;

// Base File
    cout << "\nLoading Base dataset..." << endl;
    auto start = chrono::high_resolution_clock::now();
    vector<vector<float>> points_f = databin_read(argv[6]);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> base_f_duration = end - start;
    cout << "Loaded " << points_f.size() << " points from the Base dataset in " << base_f_duration.count() << " seconds." << endl;

    // Check size of points_f
    if (points_f.empty()) {
        cout << "No base vectors read from the file." << endl;
        return EXIT_FAILURE;
    }
    vector<vector<double>> points = convert_to_double(points_f);

// Groundtruth File
    cout << "Loading Groundtruth dataset..." << endl;
    start = chrono::high_resolution_clock::now();
    vector<vector<int>> gt = gtbin_read(argv[7], k);
    end = chrono::high_resolution_clock::now();
    chrono::duration<double> ground_truth_duration = end - start;
    cout << "Loaded " << gt.size() << " groundtruth sets from the Groundtruth dataset in " << ground_truth_duration.count() << " seconds." << endl;

    // Check size of ground_truth
    if (gt.empty()) {
        cout << "No ground_truth sets read from the file." << endl;
        return EXIT_FAILURE;
    }
    
// set F_all_filters
    set<int> F_all_filters;
    for(u_int i = 0; i < points.size(); i++) {
        vector<double> point = points[i];
        F_all_filters.insert(point[0]);
    }
   
// FilteredVamana
    map<int, Node *> M;
    cout << "\nCalling FilteredVamana..." << endl;
    start = chrono::high_resolution_clock::now();
    Graph graph = filteredVamana(points, a, L, R, F_all_filters, taph, M);
    end = chrono::high_resolution_clock::now();
    chrono::duration<double> filtered_vamana_duration = end - start;
    cout << "FilteredVamana took " << filtered_vamana_duration.count() << " seconds.\n" << endl;

    cout << "Saving the graph to 'filtered_graph.bin' and the map of medoids in 'filtered_map.bin' ..." << endl;
    save_graph_to_binary(graph, "filtered_graph");
    save_map_to_binary(M, "filtered_map");

    cout << "Graph and map saved successfully! Cleaning..." << endl;
    graph.clear();

    cout << "Bye from filtered_main_graph" << endl;
    
    return 0;
}