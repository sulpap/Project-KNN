#include "../include/bin_read.hpp"
#include "../include/graph.hpp"
#include "../include/utility.hpp"
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
    auto total_start = chrono::high_resolution_clock::now();

    if (argc != 5) {
        cout << "Usage: " << argv[0] << " <k> <graph_filename> <queries_file_path> <groundtruth_file_path> " << endl;
        cout << "Note: k must be an int\n";
        return 1;
    }

    cout << "\nProcedure that loads the graph and implements the query calculations starting..." << endl;

    int k = atoi(argv[1]);
    string graph_filename = argv[2];

    // find L from filename
    int L = -1; // Default value in case L is not found
    size_t pos_L = graph_filename.find("L=");

    if (pos_L != string::npos) {
        size_t pos_start = pos_L + 2;  // Skip past "L="
        size_t pos_end = graph_filename.find('_', pos_start);
        if (pos_end == string::npos) {
            pos_end = graph_filename.length(); // in case "L=" is the last part
        }
        string L_str = graph_filename.substr(pos_start, pos_end - pos_start);
        L = stoi(L_str); // Convert substring to integer
    }

// Query File
    cout << "Loading Query dataset..." << endl;
    auto start = chrono::high_resolution_clock::now();
    vector<vector<float>> queries_f = queriesbin_read(argv[3]);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> query_f_duration = end - start;
    cout << "Loaded " << queries_f.size() << " query points from the Query dataset in " << query_f_duration.count() << " seconds." << endl;

    // Check size of query_f
    if (queries_f.empty()) {
        cout << "No queries vectors read from the file." << endl;
        return EXIT_FAILURE;
    }
    vector<vector<double>> queries = convert_to_double(queries_f);

// Groundtruth File
    cout << "Loading Groundtruth dataset..." << endl;
    start = chrono::high_resolution_clock::now();
    vector<vector<int>> gt = gtbin_read(argv[4], k);
    end = chrono::high_resolution_clock::now();
    chrono::duration<double> ground_truth_duration = end - start;
    cout << "Loaded " << gt.size() << " groundtruth sets from the Groundtruth dataset in " << ground_truth_duration.count() << " seconds." << endl;

    // Check size of ground_truth
    if (gt.empty()) {
        cout << "No ground_truth sets read from the file." << endl;
        return EXIT_FAILURE;
    }

    Graph graph = load_graph_from_binary(graph_filename);
    map<int, Node *> M = load_map_from_binary("filtered_map", graph);

    // set F_all_filters
    set<int> F_all_filters;
    for (const auto& [id, node] : graph.getAdjList())
    {
        F_all_filters.insert(node->getLabel());
    }

    // Greedy on every query
    int unfiltered_q_counter = 0;           // Μετράμε το πλήθος των unfiltered queries
    int filtered_q_counter = 0;             // Μετράμε το πλήθος των filtered queries
    float sum_percent = 0.0;
    float sum_percent_filtered = 0.0;
    float sum_percent_unfiltered = 0.0;
    float sum_greedy = 0.0;
    float sum_greedy_filtered = 0.0;
    float sum_greedy_unfiltered = 0.0;

    for(u_int i = 0; i < queries.size(); i++) {
        vector<int> gt_sol = gt[i];
        vector<double> query = queries[i];
        int type = query[0];
        int filter_q = query[1];
        vector<double> queryCoords(query.begin() + 2, query.end());

        set<Node*> S_set;
        set<Node*> L_set;
        set<Node*> V_set;
        set<int> F_set;
        if(type == 1) {
            filtered_q_counter += 1;
            // [1] Συμπλήρωση του S_set
            if (M.find(filter_q) != M.end()) {     // found
                S_set.insert(M[filter_q]);
            }
            // else S will be left out empty

            // [2] Συμπλήρωση του F_set
            F_set.insert(filter_q);
        } else {                            // type == 0
            unfiltered_q_counter += 1;
            // [1] Συμπλήρωση του S_set
            for (const auto& [filter, start_node] : M) {
                S_set.insert(start_node);
            }
            // [2] Συμπλήρωση του F_set
            F_set = F_all_filters;
        }

        cout << "Calling FilteredGreedySearch for " << i << "th query..." << endl;
        auto start = chrono::high_resolution_clock::now();
        FilteredGreedySearch(S_set, queryCoords, k, L, L_set, V_set, F_set);
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = end - start;

        // We traverse L_set
        int found = 0;
        for (auto node : L_set) {
            int id_L = node->getId();
            if (find(gt_sol.begin(), gt_sol.end(), id_L) != gt_sol.end()) {
                found++;
            }
        }
   
        float percent;
        if(gt_sol.size() == 0) {        // Στη λύση υπάρχουν 0 γείτονες
            if(found == 0)              // Και βρήκαμε και εμείς 0 γείτονες
                percent = 100.0;
            else                        // Δεν βρήκαμε 0 γείτονες
                percent = 0.0;
        } else {
            percent = (100 * found) / (float)gt_sol.size();
        }

        if(type == 1) {
            sum_percent_filtered += percent;
            sum_greedy_filtered += static_cast<float>(duration.count());        // we use duration.count() to get the time in seconds
        }
        else {            // type == 0
            sum_percent_unfiltered += percent;  
            sum_greedy_unfiltered += static_cast<float>(duration.count());
        }

        sum_percent += percent;
        sum_greedy += static_cast<float>(duration.count());
    }

    cout << "Had " << sum_percent / (float)queries.size() << "% success on filtered and unfiltered queries combined" << endl;
    cout << "Had " << sum_percent_filtered / (float)filtered_q_counter << "% success on filtered queries" << endl;
    cout << "Had " << sum_percent_unfiltered / (float)unfiltered_q_counter << "% success on unfiltered queries" << endl;
    cout << "Greedy time per query (filtered, unfiltered combined) = " << sum_greedy / (float)queries.size() << " seconds" << endl;
    cout << "Greedy time per filtered query = " << sum_greedy_filtered / (float)filtered_q_counter << " seconds" << endl;
    cout << "Greedy time per unfiltered query = " << sum_greedy_unfiltered / (float)unfiltered_q_counter << " seconds" << endl;

    graph.clear();

    auto total_end = chrono::high_resolution_clock::now();
    chrono::duration<double> total_duration = total_end - total_start;
    cout << "\nProgram ran in " << total_duration.count() << " seconds or " << total_duration.count() / 60 << " minutes." << endl;
    cout << "\nBye from main_load!" << endl;
    return 0;
}