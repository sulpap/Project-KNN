#include "../include/bin_read.hpp"
#include "../include/graph.hpp"
#include "../include/utility.hpp"
#include "../include/filteredVamana.hpp"
#include "../include/filteredGreedySearch.hpp"

#include <chrono>
#include <algorithm>    // find
#include <cassert>
#include <set>
#include <cstdlib>      // atoi
#include <iostream>
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
    int k = atoi(argv[1]);
    int L = atoi(argv[2]);
    int R = atoi(argv[3]);
    double a = stod(argv[4]);
    int taph = atoi(argv[5]);

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

// Query File
    cout << "Loading Query dataset..." << endl;
    start = chrono::high_resolution_clock::now();
    vector<vector<float>> queries_f = queriesbin_read(argv[7]);
    end = chrono::high_resolution_clock::now();
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
    vector<vector<int>> gt = gtbin_read(argv[8], k);
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

    cout << "\nFilteredVamana took " << filtered_vamana_duration.count() << " seconds." << endl;
    cout << "Had " << sum_percent / (float)queries.size() << "% success on filtered and unfiltered queries combined" << endl;
    cout << "Had " << sum_percent_filtered / (float)filtered_q_counter << "% success on filtered queries" << endl;
    cout << "Had " << sum_percent_unfiltered / (float)unfiltered_q_counter << "% success on unfiltered queries" << endl;
    cout << "Greedy time per query (filtered, unfiltered combined) = " << sum_greedy / (float)queries.size() << " seconds" << endl;
    cout << "Greedy time per filtered query = " << sum_greedy_filtered / (float)filtered_q_counter << " seconds" << endl;
    cout << "Greedy time per unfiltered query = " << sum_greedy_unfiltered / (float)unfiltered_q_counter << " seconds" << endl;

    graph.clear();

    auto total_end = chrono::high_resolution_clock::now();
    chrono::duration<double> total_duration = total_end - total_start;
    cout << "Program ran in " << total_duration.count() << " seconds or " << total_duration.count() / 60 << " minutes." << endl;
    cout << "Bye from main" << endl;
    return 0;
}

// MAIN Γιώργος:

// #include "../include/graph.hpp"
// #include "../include/utility.hpp"
// #include "../include/bin_read.hpp"
// #include "../include/filteredVamana.hpp"
// #include "../include/stitchedVamana.hpp"
// #include "../include/filteredGreedySearch.hpp"
// #include <cassert>
// #include <algorithm>        // due to use of find()
// #include <iostream>

// #include <chrono>

// using namespace std;

// int main(int argc, char* argv[]) {
//     auto total_start = chrono::high_resolution_clock::now();

//     if (argc != 9) {
//         cout << "Usage: " << argv[0] << " <k> <L> <R> <a> <t> <base_file_path> <queries_file_path> <groundtruth_file_path> " << endl;
//         cout << "Note: k must be an int\n";
//         cout << "      L must be an int\n";
//         cout << "      R must be an int\n";
//         cout << "      a must be a double\n";
//         cout << "      t must be an int" << endl;
//         // cout << "      approach must be either \"Filtered\" or \"Stitched\"" << endl;
//         return 1;
//     }

//     chrono::duration<double> base_f_duration;
//     chrono::duration<double> query_f_duration;
//     chrono::duration<double> ground_truth_duration;
//     chrono::duration<double> filtered_vamana_duration;
//     chrono::duration<double> average_query_greedy_duration;
//     chrono::duration<double> average_filtered_query_greedy_duration;
//     chrono::duration<double> average_unfiltered_query_greedy_duration;
//     chrono::duration<double> total_query_greedy_duration = chrono::duration<double>::zero();
//     chrono::duration<double> total_filtered_query_greedy_duration = chrono::duration<double>::zero();
//     chrono::duration<double> total_unfiltered_query_greedy_duration = chrono::duration<double>::zero();
    
//     // 1. Get Inputs
//     int k = stoi(argv[1]);
//     int L = stoi(argv[2]);
//     int R = stoi(argv[3]);
//     double a = stod(argv[4]);
//     int t = stoi(argv[5]);
//     // const char* approach = argv[6];
//     const char* base_filename = argv[6];
//     const char* query_filename = argv[7];
//     const char* groudtruth_filename = argv[8];

//     cout << "Arguments:\n";
//     cout << "\t- k: " << k << "\n";
//     cout << "\t- L: " << L << "\n";
//     cout << "\t- R: " << R << "\n";
//     cout << "\t- a: " << a << "\n";
//     cout << "\t- t: " << t << "\n";
//     // cout << "\t- Approach: " << approach << "\n";
//     cout << "\t- Base file: " << base_filename << "\n";
//     cout << "\t- Query file: " << query_filename << "\n";
//     cout << "\t- Groudtruth file: " << groudtruth_filename << endl;

//     // 2. Read Files
//     // Base File
//     cout << "\nLoading Base dataset..." << endl;
//     auto start = chrono::high_resolution_clock::now();
//     vector<vector<float>> base_f = databin_read(base_filename);
//     auto end = chrono::high_resolution_clock::now();
//     base_f_duration = end - start;
//     cout << "Loaded " << base_f.size() << " points from the Base dataset in " << base_f_duration.count() << " seconds." << endl;

//     // Check size of base_f
//     if (base_f.empty()) {
//         cout << "No base vectors read from the file." << endl;
//         return EXIT_FAILURE;
//     }

//     // Query File
//     cout << "Loading Query dataset..." << endl;
//     start = chrono::high_resolution_clock::now();
//     vector<vector<float>> query_f = queriesbin_read(query_filename);
//     end = chrono::high_resolution_clock::now();
//     query_f_duration = end - start;
//     cout << "Loaded " << query_f.size() << " query points from the Query dataset in " << query_f_duration.count() << " seconds." << endl;

//     // Check size of query_f
//     if (query_f.empty()) {
//         cout << "No queries vectors read from the file." << endl;
//         return EXIT_FAILURE;
//     }

//     // Groundtruth File
//     cout << "Loading Groundtruth dataset..." << endl;
//     start = chrono::high_resolution_clock::now();
//     vector<vector<int>> ground_truth = gtbin_read(groudtruth_filename, k);
//     end = chrono::high_resolution_clock::now();
//     ground_truth_duration = end - start;
//     cout << "Loaded " << ground_truth.size() << " groundtruth sets from the Groundtruth dataset in " << ground_truth_duration.count() << " seconds." << endl;

//     // Check size of ground_truth
//     if (ground_truth.empty()) {
//         cout << "No ground_truth sets read from the file." << endl;
//         return EXIT_FAILURE;
//     }
    
//     vector<vector<double>> base = convert_to_double(base_f);
//     vector<vector<double>> queries = convert_to_double(query_f);

//     // Make set_F
//     set<int> set_F;
//     for (vector<double> point : base) {
//         set_F.insert(static_cast<int>(point[0]));
//     }

//     // 3. Call Vamana.
//     Graph graph;
//     map<int, Node *> st_f;
//     cout << "\nCalling FilteredVamana..." << endl;
//     start = chrono::high_resolution_clock::now();
//     // int medoid_id = Vamana(graph, base, R, a, L, 0);
//     graph = filteredVamana(base, a, L, R, set_F, t, st_f);
//     end = chrono::high_resolution_clock::now();
//     filtered_vamana_duration = end - start;
//     cout << "FilteredVamana took " << filtered_vamana_duration.count() << " seconds.\n" << endl;

//     // Node* medoid = graph.getNode(medoid_id);
//     // cout << "Start Node for Every Filter:\n";
//     // // for
//     // cout << "\t- filter:\n"
//     // cout << "\t\t- Id: " << medoid->getId() << "\n";
//     // cout << "\t\t- GraphId: " << medoid->getGraphId() << "\n";
//     // cout << "\t\t- Coordinates:";
//     // for (double i: medoid->getCoordinates()) {
//     //     cout << " " << i;
//     // }
//     // cout << endl;

//     // 4. Call Greedy for every query.
//     // ALL
//     int totalFound = 0;
//     int totalK = 0;
//     float totalPercent = 0;
//     size_t totalQueriesSize = 0;
//     // FILTERED
//     int totalFilteredFound = 0;
//     int totalFilteredK = 0;
//     float totalFilteredPercent = 0;
//     size_t totalFilteredQueriesSize = 0;
//     // UNFILTERED
//     int totalUnfilteredFound = 0;
//     int totalUnfilteredK = 0;
//     float totalUnfilteredPercent = 0;
//     size_t totalUnfilteredQueriesSize = 0;

//     int starting_k = k;

//     set<Node *> full_S_set;
//     for (const auto& pair : st_f) {
//         full_S_set.insert(pair.second);
//     }

//     // for(int i = 0; i < static_cast<int>(queries.size()); i++) {
//     for(int i = 0; i < 100; i++) {
//         vector<double> query = queries[i];
//         set<Node*> L_set;
//         set<Node*> V_set;
//         set<Node*> S_set;
//         set<int> F_q_set;
//         vector<int> gt_sol = ground_truth[i];
//         int gt_size = static_cast<int>(gt_sol.size());
//         if (starting_k > gt_size) {
//             k = gt_size;
//         } else {
//             k = starting_k;
//         }

//         // Πρέπει να κάνω μια if για τα φίλτρα του query, ώστε να καθορίζω τα S_set και F_q_set για τον fgs
//         // πως παίρνω το φίλτρο από το query. Ποια η διαφορά type 0 vs 1

//         // int query_F = query[1];

//         // if (query_F == -1) {
//         //     S_set = full_S_set;
//         //     F_q_set = set_F;
//         // } else {
//         //     S_set.insert(st_f[query_F]);
//         //     F_q_set.insert(query_F);
//         // }

//         int query_F = query[1];

//         if (query_F == -1) {
//             S_set = full_S_set;
//             F_q_set = set_F;
//         } else {
//             if (st_f.find(query_F) != st_f.end()) {     // found
//                 S_set.insert(st_f[query_F]);
//             }
//             // else: st_f not found, and so S_set will be empty (there exists no start node with filter = query_F)
//                 // S_set will be empty
//             F_q_set.insert(query_F);
//         }

//         cout << "Calling FilteredGreedySearch for " << i << "th query..." << endl;
//         start = chrono::high_resolution_clock::now();
//         FilteredGreedySearch(S_set, query, starting_k, L, L_set, V_set, F_q_set);
//         end = chrono::high_resolution_clock::now();
//         chrono::duration<double> duration = end - start;
//         total_query_greedy_duration += duration;
//         if (query_F == -1) {
//             total_unfiltered_query_greedy_duration += duration;
//         } else {
//             total_filtered_query_greedy_duration += duration;
//         }
//         cout << "GreedySearch for " << i << "th query took " << duration.count() << " seconds." << endl;

//         // 6. Compare greedy with ground truth
//         // Paizei na min xreiazetai
//         // if (k > static_cast<int>(gt_sol.size())) {
//         //     cout << "ERROR: For query (zero based) #"<< i << " can't compare greedy vs ground truth. Could compare with k up to " << gt_sol.size() << endl;
//         //     return EXIT_FAILURE;
//         // }
//         // we only need the top k results of ground truth to compare
//         // vector<int> gt(gt_sol.begin(), gt_sol.begin() + k);

//         // We traverse L_set
//         int found = 0;
//         for (auto node : L_set) {
//             int id_L = node->getId();
//             if (find(gt_sol.begin(), gt_sol.end(), id_L) != gt_sol.end()) {
//                 found++;
//             }
//         }

//         float percent;

//         if (k == 0) {
//             if (found == 0) {
//                 percent = 100.0;
//             } else {
//                 percent = 0.0;
//             }
//         } else {
//             float percent = (100 * found) / (float)k;
//         }
//         cout << "Query (zero based) #" << i << " had " << percent << "% recall." << endl;
    
//         // Accumulate totals for overall and average recall
//         totalFound += found;
//         totalK += k;
//         totalPercent += percent;
//         totalQueriesSize++;

//         if (query_F == -1) {
//             totalFilteredFound += found;
//             totalFilteredK += k;
//             totalFilteredPercent += percent;
//             totalFilteredQueriesSize++;
//         } else {
//             totalUnfilteredFound += found;
//             totalUnfilteredK += k;
//             totalUnfilteredPercent += percent;
//             totalUnfilteredQueriesSize++;
//         }
//     }

//     // Calculate total recall across all queries
//     cout << "\nRecalls:\n";

//     float totalRecall = (100 * totalFound) / static_cast<float>(totalK);
//     cout << "\t- Total recall across ALL queries (totalFound/totalK): " << totalRecall << "%" << endl;

//     float averageRecall = totalPercent / totalQueriesSize;
//     cout << "\t- Average recall across ALL queries (totalPercent/number_of_all_queries): " << averageRecall << "%" << endl;

//     float totalFilteredRecall = (100 * totalFilteredFound) / static_cast<float>(totalFilteredK);
//     cout << "\t- Total recall across FILTERED queries (totalFilteredFound/totalFilteredK): " << totalFilteredRecall << "%" << endl;

//     float averageFilteredRecall = totalFilteredPercent / totalFilteredQueriesSize;
//     cout << "\t- Average recall across FILTERED queries (totalFilteredPercent/number_of_filtered_queries): " << averageFilteredRecall << "%" << endl;

//     float totalUnfilteredRecall = (100 * totalUnfilteredFound) / static_cast<float>(totalUnfilteredK);
//     cout << "\t- Total recall across UNFILTERED queries (totalUnfilteredFound/totalUnfilteredK): " << totalUnfilteredRecall << "%" << endl;

//     float averageUnfilteredRecall = totalUnfilteredPercent / totalUnfilteredQueriesSize;
//     cout << "\t- Average recall across UNFILTERED queries (totalUnfilteredPercent/number_of_unfiltered_queries): " << averageUnfilteredRecall << "%\n" << endl;

//     // 5. Print Summary
//     average_query_greedy_duration = total_query_greedy_duration / totalQueriesSize;
//     average_filtered_query_greedy_duration = total_filtered_query_greedy_duration / totalFilteredQueriesSize;
//     average_unfiltered_query_greedy_duration = total_unfiltered_query_greedy_duration / totalUnfilteredQueriesSize;

//     cout << "Timing Summary:\n";
//     cout << "\t- Base dataset load time: " << base_f_duration.count() << "seconds.\n";
//     cout << "\t- Query dataset load time: " << query_f_duration.count() << "seconds.\n";
//     cout << "\t- Groundtruth dataset load time: " << ground_truth_duration.count() << "seconds.\n";
//     cout << "\t- Index build time (FilteredVamana): " << filtered_vamana_duration.count() << "seconds.\n";
//     cout << "\t- Average time GreadySearch ran for ALL queries: " << average_query_greedy_duration.count() << " seconds.\n";
//     cout << "\t- Average time GreadySearch ran for FILTERED queries: " << average_filtered_query_greedy_duration.count() << " seconds.\n";
//     cout << "\t- Average time GreadySearch ran for UNFILTERED queries: " << average_unfiltered_query_greedy_duration.count() << " seconds.\n" << endl;
    
//     // 6. Clean up
//     cout << "Cleaning...\n" << endl;
//     graph.clear();      // once done

//     auto total_end = chrono::high_resolution_clock::now();
//     chrono::duration<double> total_duration = total_end - total_start;
//     cout << "\nProgram ran for " << total_duration.count() << " seconds or " << total_duration.count() / 60 << " minutes.\n" << endl;

//     cout << "Bye from main" << endl;

//     return 0;
// }



// OLD MAIN - Παραδοτέο 1


// #include "../include/graph.hpp"
// #include "../include/utility.hpp"
// #include "../include/fvecs_read.hpp"
// #include "../include/ivecs_read.hpp"
// #include "../include/vamana.hpp"
// #include "../include/greedysearch.hpp"
// #include <cassert>
// #include <algorithm>        // due to use of find()
// #include <iostream>

// #include <chrono>

// using namespace std;

// int main(int argc, char* argv[]) {
//     auto total_start = chrono::high_resolution_clock::now();

//     if (argc != 8) {
//         cout << "Usage: " << argv[0] << " <k> <L> <R> <a> <base_file_path> <queries_file_path> <groundtruth_file_path> " << endl;
//         cout << "Note: k must be an int\n";
//         cout << "      L must be an int\n";
//         cout << "      R must be an int\n";
//         cout << "      a must be a double" << endl;
//         return 1;
//     }

//     chrono::duration<double> base_f_duration;
//     chrono::duration<double> query_f_duration;
//     chrono::duration<double> ground_truth_duration;
//     chrono::duration<double> vamana_duration;
//     chrono::duration<double> average_query_greedy_duration;
//     chrono::duration<double> total_query_greedy_duration = chrono::duration<double>::zero();
    
//     // 1. Get Inputs
//     int k = stoi(argv[1]);
//     int L = stoi(argv[2]);
//     int R = stoi(argv[3]);
//     double a = stod(argv[4]);
//     const char* base_filename = argv[5];
//     const char* query_filename = argv[6];
//     const char* groudtruth_filename = argv[7];

//     cout << "Arguments:\n";
//     cout << "\t- k: " << k << "\n";
//     cout << "\t- L: " << L << "\n";
//     cout << "\t- R: " << R << "\n";
//     cout << "\t- a: " << a << "\n";
//     cout << "\t- Base file: " << base_filename << "\n";
//     cout << "\t- Query file: " << query_filename << "\n";
//     cout << "\t- Groudtruth file: " << groudtruth_filename << endl;

//     // 2. Read Files
//     // Base File
//     cout << "Loading Base dataset..." << endl;
//     auto start = chrono::high_resolution_clock::now();
//     vector<vector<float>> base_f = fvecs_read(base_filename);
//     auto end = chrono::high_resolution_clock::now();
//     base_f_duration = end - start;
//     cout << "Loaded " << base_f.size() << " points from the Base dataset in " << base_f_duration.count() << " seconds." << endl;

//     // Check size of base_f
//     if (base_f.empty()) {
//         cout << "No base vectors read from the file." << endl;
//         return EXIT_FAILURE;
//     }

//     // Query File
//     cout << "Loading Query dataset..." << endl;
//     start = chrono::high_resolution_clock::now();
//     vector<vector<float>> query_f = fvecs_read(query_filename);
//     end = chrono::high_resolution_clock::now();
//     query_f_duration = end - start;
//     cout << "Loaded " << query_f.size() << " query points from the Query dataset in " << query_f_duration.count() << " seconds." << endl;

//     // Check size of query_f
//     if (query_f.empty()) {
//         cout << "No queries vectors read from the file." << endl;
//         return EXIT_FAILURE;
//     }

//     // Groundtruth File
//     cout << "Loading Groundtruth dataset..." << endl;
//     start = chrono::high_resolution_clock::now();
//     vector<vector<int>> ground_truth = ivecs_read(groudtruth_filename);
//     end = chrono::high_resolution_clock::now();
//     ground_truth_duration = end - start;
//     cout << "Loaded " << ground_truth.size() << " groundtruth sets from the Groundtruth dataset in " << ground_truth_duration.count() << " seconds." << endl;

//     // Check size of ground_truth
//     if (ground_truth.empty()) {
//         cout << "No ground_truth sets read from the file." << endl;
//         return EXIT_FAILURE;
//     }

//     vector<vector<double>> base = convert_to_double(base_f);
//     vector<vector<double>> queries = convert_to_double(query_f);

//     // 3. Call Vamana.
//     Graph graph;
//     cout << "Calling Vamana..." << endl;
//     start = chrono::high_resolution_clock::now();
//     int medoid_id = Vamana(graph, base, R, a, L, 0);//added a label to not get errors
//     end = chrono::high_resolution_clock::now();
//     vamana_duration = end - start;
//     cout << "Vamana took " << vamana_duration.count() << " seconds." << endl;

//     Node* medoid = graph.getNode(medoid_id);
//     cout << "Medoid Node: \n";
//     cout << "\t- Id: " << medoid->getId() << "\n";
//     cout << "\t- GraphId: " << medoid->getGraphId() << "\n";
//     cout << "\t- Coordinates:";
//     for (double i: medoid->getCoordinates()) {
//         cout << " " << i;
//     }
//     cout << endl;

//     // 4. Call Greedy for every query.
//     int totalFound = 0;
//     int totalK = 0;
//     float totalPercent = 0;

//     for(int i = 0; i < static_cast<int>(queries.size()); i++) {
//         vector<double> query = queries[i];
//         set<Node*> L_set;
//         set<Node*> V_set;

//         cout << "Calling GreedySearch for " << i << "th query..." << endl;
//         start = chrono::high_resolution_clock::now();
//         GreedySearch(medoid, query, k, L, L_set, V_set);
//         end = chrono::high_resolution_clock::now();
//         chrono::duration<double> duration = end - start;
//         total_query_greedy_duration += duration;
//         cout << "GreedySearch for " << i << "th query took " << duration.count() << " seconds." << endl;

//         // 6. Compare greedy with ground truth
//         vector<int> gt_sol = ground_truth[i];
//         if (k > static_cast<int>(gt_sol.size())) {
//             cout << "ERROR: For query (zero based) #"<< i << " can't compare greedy vs ground truth. Could compare with k up to " << gt_sol.size() << endl;
//             return EXIT_FAILURE;
//         }
//         // we only need the top k results of ground truth to compare
//         vector<int> gt(gt_sol.begin(), gt_sol.begin() + k);

//         // We traverse L_set
//         int found = 0;
//         for (auto node : L_set) {
//             int id_L = node->getId();
//             if (find(gt.begin(), gt.end(), id_L) != gt.end()) {
//                 found++;
//             }
//         }

//         float percent = (100 * found) / (float)k;
//         cout << "Query (zero based) #" << i << " had " << percent << "% recall." << endl;
    
//         // Accumulate totals for overall and average recall
//         totalFound += found;
//         totalK += k;
//         totalPercent += percent;
//     }

//     // Calculate total recall across all queries
//     cout << "Recalls:\n";

//     float totalRecall = (100 * totalFound) / static_cast<float>(totalK);
//     cout << "\t- Total recall across all queries (totalFound/totalK): " << totalRecall << "%" << endl;

//     float averageRecall = totalPercent / queries.size();
//     cout << "\t- Average recall across all queries (totalPercent/number_of_queries): " << averageRecall << "%" << endl;

//     // 5. Print Summary
//     average_query_greedy_duration = total_query_greedy_duration / queries.size();

//     cout << "Timing Summary:\n";
//     cout << "\t- Base dataset load time: " << base_f_duration.count() << "seconds.\n";
//     cout << "\t- Query dataset load time: " << query_f_duration.count() << "seconds.\n";
//     cout << "\t- Groundtruth dataset load time: " << ground_truth_duration.count() << "seconds.\n";
//     cout << "\t- Index build time (Vamana): " << vamana_duration.count() << "seconds.\n";
//     cout << "\t- Average time GreadySearch ran for queries: " << average_query_greedy_duration.count() << "seconds.\n";
    
//     // 6. Clean up
//     cout << "Cleaning..." << endl;
//     graph.clear();      // once done

//     auto total_end = chrono::high_resolution_clock::now();
//     chrono::duration<double> total_duration = total_end - total_start;
//     cout << "\nProgram ran for " << total_duration.count() << " seconds or " << total_duration.count() / 60 << " minutes.\n" << endl;

//     cout << "Bye from main" << endl;

//     return 0;
// }

// /*
// [0] Examples of execution (Must be in the directory that Makefile is):
// ./bin/main 20 40 80 1.2 datasets/siftsmall/siftsmall_base.fvecs datasets/siftsmall/siftsmall_query.fvecs datasets/siftsmall/siftsmall_groundtruth.ivecs

// ./bin/main 100 200 60 1.2 datasets/siftsmall/siftsmall_base.fvecs datasets/siftsmall/siftsmall_query.fvecs datasets/siftsmall/siftsmall_groundtruth.ivecs

// */
