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
    if (argc != 6) 
    {
        cout << "Usage: " << argv[0] << " <L> <R> <a> <t> <base_file_path>" << endl;
        cout << "Note: L must be an int\n";
        cout << "      R must be an int\n";
        cout << "      a must be a double\n";
        cout << "      t must be an int" << endl;
        return 1;
    }

    cout << "\nFilteredVamana procedure that saves the graph starting..." << endl;

    int L = atoi(argv[1]);
    int R = atoi(argv[2]);
    double a = stod(argv[3]);
    int taph = atoi(argv[4]);

// Base File
    cout << "\nLoading Base dataset..." << endl;
    auto start = chrono::high_resolution_clock::now();
    vector<vector<float>> points_f = databin_read(argv[5]);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> base_f_duration = end - start;
    cout << "Loaded " << points_f.size() << " points from the Base dataset in " << base_f_duration.count() << " seconds." << endl;

    // Check size of points_f
    if (points_f.empty()) {
        cout << "No base vectors read from the file." << endl;
        return EXIT_FAILURE;
    }
    vector<vector<double>> points = convert_to_double(points_f);
    
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

    // save the graph and the map to binary files. graph file contains vamana's parameters in its name, so we can know their values at all times
    // to call filtered_main_load with the correct parameters
    string graph_filename = "filtered_graph_L=" + to_string(L) + "_a=" + to_string(a) + "_R=" + to_string(R) + "_taph=" + to_string(taph);
    cout << "Saving the graph to '" << graph_filename << ".bin' and the map of medoids in 'filtered_map.bin' ..." << endl;
    save_graph_to_binary(graph, graph_filename);
    save_map_to_binary(M, "filtered_map");

    cout << "\nGraph and map saved successfully! Cleaning..." << endl;
    graph.clear();

    cout << "\nBye from filtered_main_graph!" << endl;
    
    return 0;
}