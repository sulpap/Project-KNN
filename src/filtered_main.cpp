#include "../include/graph.hpp"
#include "../include/utility.hpp"
#include "../include/bin_read.hpp"
#include "../include/filteredVamana.hpp"
#include "../include/stitchedVamana.hpp"
#include "../include/filteredGreedySearch.hpp"
#include <cassert>
#include <algorithm>        // due to use of find()
#include <iostream>

#include <chrono>

using namespace std;

int main(int argc, char* argv[]) {
    auto total_start = chrono::high_resolution_clock::now();

    if (argc != 9) {
        cout << "Usage: " << argv[0] << " <k> <L> <R> <a> <t> <base_file_path> <queries_file_path> <groundtruth_file_path> " << endl;
        cout << "Note: k must be an int\n";
        cout << "      L must be an int\n";
        cout << "      R must be an int\n";
        cout << "      a must be a double\n";
        cout << "      t must be an int" << endl;
        return 1;
    }

    chrono::duration<double> base_f_duration;
    chrono::duration<double> query_f_duration;
    chrono::duration<double> ground_truth_duration;
    chrono::duration<double> filtered_vamana_duration;
    chrono::duration<double> average_query_greedy_duration;
    chrono::duration<double> average_filtered_query_greedy_duration;
    chrono::duration<double> average_unfiltered_query_greedy_duration;
    chrono::duration<double> total_query_greedy_duration = chrono::duration<double>::zero();
    chrono::duration<double> total_filtered_query_greedy_duration = chrono::duration<double>::zero();
    chrono::duration<double> total_unfiltered_query_greedy_duration = chrono::duration<double>::zero();
    
    // 1. Get Inputs
    int k = stoi(argv[1]);
    int L = stoi(argv[2]);
    int R = stoi(argv[3]);
    double a = stod(argv[4]);
    int t = stoi(argv[5]);
    const char* base_filename = argv[6];
    const char* query_filename = argv[7];
    const char* groudtruth_filename = argv[8];

    cout << "Arguments:\n";
    cout << "\t- k: " << k << "\n";
    cout << "\t- L: " << L << "\n";
    cout << "\t- R: " << R << "\n";
    cout << "\t- a: " << a << "\n";
    cout << "\t- t: " << t << "\n";
    cout << "\t- Base file: " << base_filename << "\n";
    cout << "\t- Query file: " << query_filename << "\n";
    cout << "\t- Groudtruth file: " << groudtruth_filename << endl;

    // 2. Read Files
    // Base File
    cout << "\nLoading Base dataset..." << endl;
    auto start = chrono::high_resolution_clock::now();
    vector<vector<float>> base_f = databin_read(base_filename);
    auto end = chrono::high_resolution_clock::now();
    base_f_duration = end - start;
    cout << "Loaded " << base_f.size() << " points from the Base dataset in " << base_f_duration.count() << " seconds." << endl;

    // Check size of base_f
    if (base_f.empty()) {
        cout << "No base vectors read from the file." << endl;
        return EXIT_FAILURE;
    }

    // Query File
    cout << "Loading Query dataset..." << endl;
    start = chrono::high_resolution_clock::now();
    vector<vector<float>> query_f = queriesbin_read(query_filename);
    end = chrono::high_resolution_clock::now();
    query_f_duration = end - start;
    cout << "Loaded " << query_f.size() << " query points from the Query dataset in " << query_f_duration.count() << " seconds." << endl;

    // Check size of query_f
    if (query_f.empty()) {
        cout << "No queries vectors read from the file." << endl;
        return EXIT_FAILURE;
    }

    // Groundtruth File
    cout << "Loading Groundtruth dataset..." << endl;
    start = chrono::high_resolution_clock::now();
    vector<vector<int>> ground_truth = gtbin_read(groudtruth_filename, k);
    end = chrono::high_resolution_clock::now();
    ground_truth_duration = end - start;
    cout << "Loaded " << ground_truth.size() << " groundtruth sets from the Groundtruth dataset in " << ground_truth_duration.count() << " seconds." << endl;

    // Check size of ground_truth
    if (ground_truth.empty()) {
        cout << "No ground_truth sets read from the file." << endl;
        return EXIT_FAILURE;
    }
    
    vector<vector<double>> base = convert_to_double(base_f);
    vector<vector<double>> queries = convert_to_double(query_f);

    // Make set_F
    set<int> set_F;
    for (vector<double> point : base) {
        set_F.insert(static_cast<int>(point[0]));
    }

    // 3. Call Vamana.
    Graph graph;
    map<int, Node *> st_f;
    cout << "\nCalling FilteredVamana..." << endl;
    start = chrono::high_resolution_clock::now();
    // int medoid_id = Vamana(graph, base, R, a, L, 0);
    graph = filteredVamana(base, a, L, R, set_F, t, st_f);
    end = chrono::high_resolution_clock::now();
    filtered_vamana_duration = end - start;
    cout << "FilteredVamana took " << filtered_vamana_duration.count() << " seconds.\n" << endl;

    // 4. Call Greedy for every query.
    // ALL
    int totalFound = 0;
    int totalK = 0;
    float totalPercent = 0;
    size_t totalQueriesSize = 0;
    // FILTERED
    int totalFilteredFound = 0;
    int totalFilteredK = 0;
    float totalFilteredPercent = 0;
    size_t totalFilteredQueriesSize = 0;
    // UNFILTERED
    int totalUnfilteredFound = 0;
    int totalUnfilteredK = 0;
    float totalUnfilteredPercent = 0;
    size_t totalUnfilteredQueriesSize = 0;

    int starting_k = k;

    set<Node *> full_S_set;
    for (const auto& pair : st_f) {
        full_S_set.insert(pair.second);
    }

    for(int i = 0; i < static_cast<int>(queries.size()); i++) {
    // for(int i = 0; i < 100; i++) {
        vector<double> query = queries[i];
        set<Node*> L_set;
        set<Node*> V_set;
        set<Node*> S_set;
        set<int> F_q_set;
        vector<int> gt_sol = ground_truth[i];
        vector<double> query_coords(query.begin() + 2, query.end());
        int gt_size = static_cast<int>(gt_sol.size());
        if (starting_k > gt_size) {
            k = gt_size;
        } else {
            k = starting_k;
        }

        int query_F = query[1];

        if (query_F == -1) {
            for (int label : set_F) {
                set<Node*> temp_S_set;
                set<int> temp_F_q_set;
                set<Node*> temp_L_set;
                set<Node*> temp_V_set;
                temp_S_set.insert(st_f[label]);
                temp_F_q_set.insert(label);

                start = chrono::high_resolution_clock::now();
                FilteredGreedySearch(temp_S_set, query_coords, 1, L, temp_L_set, temp_V_set, temp_F_q_set);
                end = chrono::high_resolution_clock::now();
                chrono::duration<double> duration = end - start;
                total_query_greedy_duration += duration;
                total_unfiltered_query_greedy_duration += duration;
                
                S_set.insert(*temp_L_set.begin());
            }
            F_q_set = set_F;
        } else {
            if (st_f.find(query_F) != st_f.end()) {     // found
                S_set.insert(st_f[query_F]);
            }
            // else: st_f not found, and so S_set will be empty (there exists no start node with filter = query_F)
                // S_set will be empty
            F_q_set.insert(query_F);
        }

        cout << "Calling FilteredGreedySearch for " << i << "th query with label " << query_F << ". . ." << endl;
        start = chrono::high_resolution_clock::now();
        FilteredGreedySearch(S_set, query_coords, starting_k, L, L_set, V_set, F_q_set);
        end = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = end - start;
        total_query_greedy_duration += duration;
        if (query_F == -1) {
            total_unfiltered_query_greedy_duration += duration;
        } else {
            total_filtered_query_greedy_duration += duration;
        }
        cout << "GreedySearch for " << i << "th query took " << duration.count() << " seconds." << endl;

        // 5. Compare greedy with ground truth

        // We traverse L_set
        int found = 0;
        for (auto node : L_set) {
            int id_L = node->getId();
            if (find(gt_sol.begin(), gt_sol.end(), id_L) != gt_sol.end()) {
                found++;
            }
        }

        float percent;

        if (k == 0) {
            if (found == 0) {
                percent = 100.0;
            } else {
                percent = 0.0;
            }
        } else {
            percent = (100 * found) / (float)k;
        }
        cout << "Query (zero based) #" << i << " had " << percent << "% recall." << endl;
    
        // Accumulate totals for overall and average recall
        totalFound += found;
        totalK += k;
        totalPercent += percent;
        totalQueriesSize++;

        if (query_F == -1) {
            totalUnfilteredFound += found;
            totalUnfilteredK += k;
            totalUnfilteredPercent += percent;
            totalUnfilteredQueriesSize++;
        } else {
            totalFilteredFound += found;
            totalFilteredK += k;
            totalFilteredPercent += percent;
            totalFilteredQueriesSize++;
        }
    }

    cout << "\nArguments:\n";
    cout << "\t- k: " << k << "\n";
    cout << "\t- L: " << L << "\n";
    cout << "\t- R: " << R << "\n";
    cout << "\t- a: " << a << "\n";
    cout << "\t- t: " << t << "\n";
    cout << "\t- Base file: " << base_filename << "\n";
    cout << "\t- Query file: " << query_filename << "\n";
    cout << "\t- Groudtruth file: " << groudtruth_filename << endl;

    cout << "\nQueries:\n";

    cout << "\t- Total: " << totalQueriesSize << endl;
    cout << "\t- Filtered: " << totalFilteredQueriesSize << endl;
    cout << "\t- Unfiltered: " << totalUnfilteredQueriesSize << endl;

    float filteredPercentage = (100 * totalFilteredQueriesSize) / totalQueriesSize;
    cout << "\t- Percentage of Filtered: " << filteredPercentage << "%" << endl;

    float unfilteredPercentage = (100 * totalUnfilteredQueriesSize) / totalQueriesSize;
    cout << "\t- Percentage of Unfiltered: " << unfilteredPercentage << "%" << endl;

    // Calculate total recall across all queries
    cout << "\nRecalls:\n";

    float totalRecall = (100 * totalFound) / static_cast<float>(totalK);
    cout << "\t- Total recall across ALL queries (totalFound/totalK): " << totalRecall << "%" << endl;

    float averageRecall = totalPercent / totalQueriesSize;
    cout << "\t- Average recall across ALL queries (totalPercent/number_of_all_queries): " << averageRecall << "%" << endl;

    float totalFilteredRecall = (100 * totalFilteredFound) / static_cast<float>(totalFilteredK);
    cout << "\t- Total recall across FILTERED queries (totalFilteredFound/totalFilteredK): " << totalFilteredRecall << "%" << endl;

    float averageFilteredRecall = totalFilteredPercent / totalFilteredQueriesSize;
    cout << "\t- Average recall across FILTERED queries (totalFilteredPercent/number_of_filtered_queries): " << averageFilteredRecall << "%" << endl;

    float totalUnfilteredRecall = (100 * totalUnfilteredFound) / static_cast<float>(totalUnfilteredK);
    cout << "\t- Total recall across UNFILTERED queries (totalUnfilteredFound/totalUnfilteredK): " << totalUnfilteredRecall << "%" << endl;

    float averageUnfilteredRecall = totalUnfilteredPercent / totalUnfilteredQueriesSize;
    cout << "\t- Average recall across UNFILTERED queries (totalUnfilteredPercent/number_of_unfiltered_queries): " << averageUnfilteredRecall << "%\n" << endl;

    // 6. Print Summary
    average_query_greedy_duration = total_query_greedy_duration / totalQueriesSize;
    average_filtered_query_greedy_duration = total_filtered_query_greedy_duration / totalFilteredQueriesSize;
    average_unfiltered_query_greedy_duration = total_unfiltered_query_greedy_duration / totalUnfilteredQueriesSize;

    cout << "Timing Summary:\n";
    cout << "\t- Base dataset load time: " << base_f_duration.count() << " seconds.\n";
    cout << "\t- Query dataset load time: " << query_f_duration.count() << " seconds.\n";
    cout << "\t- Groundtruth dataset load time: " << ground_truth_duration.count() << " seconds.\n";
    cout << "\t- Index build time (FilteredVamana): " << filtered_vamana_duration.count() << " seconds or " << filtered_vamana_duration.count() / 60 << " minutes.\n";
    cout << "\t- Total time FilteredGreadySearch calculation took for ALL queries: " << total_query_greedy_duration.count() << " seconds.\n";
    cout << "\t- Total time FilteredGreadySearch calculation took for FILTERED queries: " << total_filtered_query_greedy_duration.count() << " seconds.\n";
    cout << "\t- Total time FilteredGreadySearch calculation took for UNFILTERED queries (with the calculation of their starting nodes): " << total_unfiltered_query_greedy_duration.count() << " seconds.\n";
    cout << "\t- Average time FilteredGreadySearch took for ALL queries: " << average_query_greedy_duration.count() << " seconds.\n";
    cout << "\t- Average time FilteredGreadySearch took for FILTERED queries: " << average_filtered_query_greedy_duration.count() << " seconds.\n";
    cout << "\t- Average time FilteredGreadySearch took for UNFILTERED queries (with the calculation of their starting nodes): " << average_unfiltered_query_greedy_duration.count() << " seconds.\n" << endl;
    
    
    // 7. Clean up
    cout << "Cleaning...\n" << endl;
    graph.clear();      // once done

    auto total_end = chrono::high_resolution_clock::now();
    chrono::duration<double> total_duration = total_end - total_start;
    cout << "\nProgram ran for " << total_duration.count() << " seconds or " << total_duration.count() / 60 << " minutes.\n" << endl;

    cout << "Bye from main" << endl;

    return 0;
}