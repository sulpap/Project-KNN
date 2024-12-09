#include "../include/graph.hpp"
#include "../include/utility.hpp"
#include "../include/bin_read.hpp"
#include "../include/filteredVamana.hpp"
#include "../include/stitchedVamana.hpp"
#include "../include/filteredGreedySearch.hpp"
#include <cassert>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <sstream>
#include <map>
#include <cstring>

using namespace std;

// Function to trim whitespace
const char* trim(const char* str) {
    while (isspace(*str)) str++; // Skip leading spaces
    const char* end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) end--; // Skip trailing spaces
    *(char*)(end + 1) = '\0'; // Null-terminate
    return str;
}

// Function to parse the input file
bool parseInputFile(const char* filename, map<string, string>& params) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open the input file." << endl;
        return false;
    }

    string line;
    while (getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') continue;

        size_t pos = line.find('=');
        if (pos == string::npos) {
            cerr << "Error: Invalid line format: " << line << endl;
            return false;
        }

        string key = line.substr(0, pos);
        string value = line.substr(pos + 1);

        // Trim spaces and store in the map
        key = trim(key.c_str());
        value = trim(value.c_str());

        params[key] = value;
    }
    return true;
}

int main(int argc, char* argv[]) {
    auto total_start = chrono::high_resolution_clock::now();

    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    const char* input_file = argv[1];
    map<string, string> params;

    // Parse the input file
    if (!parseInputFile(input_file, params)) {
        return 1;
    }

    // cout << "Contents of params after parsing:" << endl;
    // for (const auto& param : params) {
    //     cout << param.first << " = " << param.second << endl;
    // }

    // if (params.find("k") == params.end()) {
    //     cerr << "Error: 'k' not found in the input file!" << endl;
    // }
    // if (params.find("L") == params.end()) {
    //     cerr << "Error: 'L' not found in the input file!" << endl;
    // }
    // if (params.find("R") == params.end()) {
    //     cerr << "Error: 'R' not found in the input file!" << endl;
    // }
    // if (params.find("a") == params.end()) {
    //     cerr << "Error: 'a' not found in the input file!" << endl;
    // }
    // if (params.find("R_stitched") == params.end()) {
    //     cerr << "Error: 'R_stitched' not found in the input file!" << endl;
    // }
    // if (params.find("base_file") == params.end()) {
    //     cerr << "Error: 'base_file' not found in the input file!" << endl;
    // }
    // if (params.find("queries_file") == params.end()) {
    //     cerr << "Error: 'queries_file' not found in the input file!" << endl;
    // }
    // if (params.find("groundtruth_file") == params.end()) {
    //     cerr << "Error: 'groundtruth_file' not found in the input file!" << endl;
    // }

    // Extract parameters
    int k, L, R, t;
    double a;
    const char* base_filename = nullptr;
    const char* query_filename = nullptr;
    const char* groundtruth_filename = nullptr;

    try {
    k = stoi(params.at("k"));
    L = stoi(params.at("L"));
    R = stoi(params.at("R"));
    a = stod(params.at("a"));
    t = stoi(params.at("t"));
    base_filename = params.at("base_file").c_str();
    query_filename = params.at("queries_file").c_str();
    groundtruth_filename = params.at("groundtruth_file").c_str();
} catch (const out_of_range& e) {
    cerr << "Error: Missing required parameter in the input file." << endl;
    return 1;
} catch (const invalid_argument& e) {
    cerr << "Error: Invalid parameter value in the input file." << endl;
    return 1;
}

    // Output the parsed arguments
    cout << "Arguments:\n";
    cout << "\t- k: " << k << "\n";
    cout << "\t- L: " << L << "\n";
    cout << "\t- R: " << R << "\n";
    cout << "\t- a: " << a << "\n";
    cout << "\t- t: " << t << "\n";
    cout << "\t- Base file: " << base_filename << "\n";
    cout << "\t- Query file: " << query_filename << "\n";
    cout << "\t- Groundtruth file: " << groundtruth_filename << endl;

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
    vector<vector<int>> ground_truth = gtbin_read(groundtruth_filename, k);
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

    int queries_size = static_cast<int>(queries.size());

    for(int i = 0; i < queries_size ; i++) {
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

        // Uncomment below if you want more details for each query
        // cout << "Calling FilteredGreedySearch for " << i << "th query with label " << query_F << ". . ." << endl;
        std::cout << "\rCalculating Query " << (i + 1) << "/" << queries_size << std::flush;

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
        // Uncomment below if you want more details for each query
        // cout << "GreedySearch for " << i << "th query took " << duration.count() << " seconds." << endl;

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
        // Uncomment below if you want more details for each query
        // cout << "Query (zero based) #" << i << " had " << percent << "% recall." << endl;
    
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

    cout << "\n\nArguments:\n";
    cout << "\t- k: " << k << "\n";
    cout << "\t- L: " << L << "\n";
    cout << "\t- R: " << R << "\n";
    cout << "\t- a: " << a << "\n";
    cout << "\t- t: " << t << "\n";
    cout << "\t- Base file: " << base_filename << "\n";
    cout << "\t- Query file: " << query_filename << "\n";
    cout << "\t- Groundtruth file: " << groundtruth_filename << endl;

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
    cout << "\t- Index build time (FilteredVamana): " << filtered_vamana_duration.count() << " second or " << filtered_vamana_duration.count() / 60 << " minutes.\n";
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