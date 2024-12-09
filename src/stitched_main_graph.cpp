#include "../include/graph.hpp"
#include "../include/utility.hpp"
#include "../include/bin_read.hpp"
#include "../include/filteredVamana.hpp"
#include "../include/stitchedVamana.hpp"
#include "../include/filteredGreedySearch.hpp"
#include "../include/graph_binary_io.hpp"
#include <cassert>
#include <algorithm>        // due to use of find()
#include <iostream>

#include <chrono>

using namespace std;

int main(int argc, char* argv[])
{
    auto total_start = chrono::high_resolution_clock::now();

    if (argc != 6) 
    {
        cout << "Usage: " << argv[0] << " <L> <R> <a> <R_stitched> <base_file_path>" << endl;
        cout << "Note: L must be an int\n";
        cout << "      R must be an int\n";
        cout << "      a must be a double\n";
        cout << "      R_stitched must be an int" << endl;
        return 1;
    }

    cout << "\nStitchedVamana procedure that saves the graph starting..." << endl;

    int L = atoi(argv[1]);
    int R = atoi(argv[2]);
    double a = stod(argv[3]);
    int R_stitched = stoi(argv[4]);

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
   
// StitchedVamana
    map<int, Node *> st_f;
    cout << "\nCalling StitchedVamana..." << endl;
    start = chrono::high_resolution_clock::now();
    Graph graph = stitchedVamana(points, F_all_filters, a, L, R, R_stitched, st_f);
    end = chrono::high_resolution_clock::now();

    // save the graph and the map to binary files. graph file contains k and L in its name, so we can know their values at all times
    // to call stitched_main_load with the correct parameters
    string graph_filename = "stitched_graph_L=" + to_string(L) + "_a=" + to_string(a) + "_R=" + to_string(R) + "_R_stitched=" + to_string(R_stitched);
    string map_filename = "stitched_map_L=" + to_string(L) + "_a=" + to_string(a) + "_R=" + to_string(R) + "_R_stitched=" + to_string(R_stitched);
    cout << "Saving the graph to './datasets/smallscale/" << graph_filename << ".bin' and the map of medoids to './datasets/smallscale/" << map_filename << ".bin' binary files ..." << endl;
    save_graph_to_binary(graph, graph_filename);
    save_map_to_binary(st_f, map_filename);

    cout << "\nGraph and map saved successfully! Cleaning..." << endl;
    graph.clear();

    auto total_end = chrono::high_resolution_clock::now();
    chrono::duration<double> total_duration = total_end - total_start;
    cout << "\nProgram ran in " << total_duration.count() << " seconds or " << total_duration.count() / 60 << " minutes." << endl;

    cout << "\nBye from stitched_main_graph!" << endl;
    
    return 0;
}