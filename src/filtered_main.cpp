#include "../include/graph.hpp"
#include "../include/utility.hpp"
#include "../include/bin_read.hpp"
#include "../include/graph_binary_io.hpp"
#include "../include/filteredVamana.hpp"
#include "../include/filteredGreedySearch.hpp"
#include <cassert>
#include <algorithm>        // due to use of find()
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <sstream>
#include <map>
#include <cstring>
#include <set>
#include <cstdlib>
#include <sys/resource.h> // for finding out memory usage

#include <chrono>

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

// function to count memory usage
size_t getMemoryUsage() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss; // returns in KB
}

int main(int argc, char* argv[]) {
    auto total_start = chrono::high_resolution_clock::now();
    auto start = chrono::high_resolution_clock::now();
    auto end = chrono::high_resolution_clock::now();

    if (argc != 9 && argc != 6 && argc != 2) {
        cout << "Usage_1: " << argv[0] << " <k> <L> <R> <a> <t> <base_file_path> <queries_file_path> <groundtruth_file_path> " << endl;
        cout << "Usage_2: " << argv[0] << " <k> <graph_filename> <map_filename> <queries_file_path> <groundtruth_file_path> " << endl;
        cout << "Usage_3: " << argv[0] << " <input_file>" << endl;
        cout << "Note: k must be an int\n";
        cout << "      L must be an int\n";
        cout << "      R must be an int\n";
        cout << "      a must be a double\n";
        cout << "      t must be an int\n";
        cout << "      graph_filename and map_filename must be the names of the files, not the paths\n";
        cout << "      input_file must be a .txt file with a specific structure like filtered_config.txt" << endl;
        return 1;
    }

    chrono::duration<double> base_f_duration;
    chrono::duration<double> query_f_duration;
    chrono::duration<double> ground_truth_duration;
    chrono::duration<double> filtered_vamana_duration = chrono::duration<double>::zero();
    chrono::duration<double> load_graph_duration;
    chrono::duration<double> load_map_duration;
    chrono::duration<double> average_query_greedy_duration;
    chrono::duration<double> average_filtered_query_greedy_duration;
    chrono::duration<double> average_unfiltered_query_greedy_duration;
    chrono::duration<double> total_query_greedy_duration = chrono::duration<double>::zero();
    chrono::duration<double> total_filtered_query_greedy_duration = chrono::duration<double>::zero();
    chrono::duration<double> total_unfiltered_query_greedy_duration = chrono::duration<double>::zero();

    size_t memoryBefore;
    size_t memoryAfter;
    size_t memoryUsed;

    int k = 0;
    int L = 0;
    int R = 0;
    int t = 0;
    double a = 0.0;
    const char* base_filename = nullptr;
    const char* query_filename = nullptr;
    const char* groundtruth_filename = nullptr;
    const char* graph_filename = nullptr;
    const char* map_filename = nullptr;
    
    // 1. Get Inputs
    if (argc == 2) {
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
        // if (params.find("t") == params.end()) {
        //     cerr << "Error: 't' not found in the input file!" << endl;
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

        const char * tmp_base_filename = params.at("base_file").c_str();
        const size_t base_filename_len = strlen(tmp_base_filename);
        char * tmp_new_base_filename = new char[base_filename_len + 1];

        const char * tmp_query_filename = params.at("queries_file").c_str();
        const size_t query_filename_len = strlen(tmp_query_filename);
        char * tmp_new_query_filename = new char[query_filename_len + 1];

        const char * tmp_groundtruth_filename = params.at("groundtruth_file").c_str();
        const size_t groundtruth_filename_len = strlen(tmp_groundtruth_filename);
        char * tmp_new_groundtruth_filename = new char[groundtruth_filename_len + 1];

        try {
            k = stoi(params.at("k"));
            L = stoi(params.at("L"));
            R = stoi(params.at("R"));
            a = stod(params.at("a"));
            t = stoi(params.at("t"));

            strncpy(tmp_new_base_filename, tmp_base_filename, base_filename_len + 1);
            tmp_new_base_filename[base_filename_len] = '\0';
            delete[] base_filename;
            base_filename = tmp_new_base_filename;
            
            strncpy(tmp_new_query_filename, tmp_query_filename, query_filename_len + 1);
            tmp_new_query_filename[query_filename_len] = '\0';
            delete[] query_filename;
            query_filename = tmp_new_query_filename;

            strncpy(tmp_new_groundtruth_filename, tmp_groundtruth_filename, groundtruth_filename_len + 1);
            tmp_new_groundtruth_filename[groundtruth_filename_len] = '\0';
            delete[] groundtruth_filename;
            groundtruth_filename = tmp_new_groundtruth_filename;
        } catch (const out_of_range& e) {
            cerr << "Error: Missing required parameter in the input file." << endl;
            return 1;
        } catch (const invalid_argument& e) {
            cerr << "Error: Invalid parameter value in the input file." << endl;
            return 1;
        }
    } else if (argc == 6) {
        k = stoi(argv[1]);
        graph_filename = argv[2];
        map_filename = argv[3];
        query_filename = argv[4];
        groundtruth_filename = argv[5];
    } else if (argc == 9) {
        k = stoi(argv[1]);
        L = stoi(argv[2]);
        R = stoi(argv[3]);
        a = stod(argv[4]);
        t = stoi(argv[5]);
        base_filename = argv[6];
        query_filename = argv[7];
        groundtruth_filename = argv[8];
    }

    cout << "Arguments:\n";
    cout << "\t- k: " << k << "\n";
    if (argc != 6) {
        cout << "\t- L: " << L << "\n";
        cout << "\t- R: " << R << "\n";
        cout << "\t- a: " << a << "\n";
        cout << "\t- t: " << t << "\n";
        cout << "\t- Base file: " << base_filename << "\n";
    } else {
        cout << "\t- Graph file: " << graph_filename << "\n";
        cout << "\t- Map file: " << map_filename << "\n";
    }
    cout << "\t- Query file: " << query_filename << "\n";
    cout << "\t- Groundtruth file: " << groundtruth_filename << endl;

    // 2. Read Files
    // Base File
    vector<vector<float>> base_f;
    if (argc != 6) {
        cout << "\nLoading Base dataset..." << endl;
        start = chrono::high_resolution_clock::now();
        base_f = databin_read(base_filename);
        end = chrono::high_resolution_clock::now();
        base_f_duration = end - start;
        cout << "Loaded " << base_f.size() << " points from the Base dataset in " << base_f_duration.count() << " seconds." << endl;

        // Check size of base_f
        if (base_f.empty()) {
            cout << "No base vectors read from the file." << endl;
            return EXIT_FAILURE;
        }
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
    
    vector<vector<double>> base;
    if (argc != 6) {
        base = convert_to_double(base_f);
    }
    vector<vector<double>> queries = convert_to_double(query_f);
    
    // Make set_F
    set<int> set_F;
    if (argc != 6) {
        for (vector<double> point : base) {
            set_F.insert(static_cast<int>(point[0]));
        }
    }

    // 3. Call Vamana or Load Graph and Map from files
    Graph graph;
    map<int, Node *> st_f;

    if (argc != 6) {
        cout << "\nCalling FilteredVamana..." << endl;
        memoryBefore = getMemoryUsage();

        start = chrono::high_resolution_clock::now();
        graph = filteredVamana(base, a, L, R, set_F, t, st_f);
        end = chrono::high_resolution_clock::now();

        memoryAfter = getMemoryUsage();
        filtered_vamana_duration = end - start;
        memoryUsed = memoryAfter - memoryBefore;
        cout << "FilteredVamana took " << filtered_vamana_duration.count() << " seconds." << endl;
        cout << "Memory used by FilteredVamana: " << memoryUsed / 1024.0 << " MB\n" << endl; // convert KB to MB
    } else {
        // find L from filename
        L = -1; // Default value in case L is not found
        string temp_graph_filename = graph_filename;
        string temp_map_filename = map_filename;
        size_t pos_L = temp_graph_filename.find("L=");

        if (pos_L != string::npos) {
            size_t pos_start = pos_L + 2;  // Skip past "L="
            size_t pos_end = temp_graph_filename.find('_', pos_start);
            if (pos_end == string::npos) {
                pos_end = temp_graph_filename.length(); // in case "L=" is the last part
            }
            string L_str = temp_graph_filename.substr(pos_start, pos_end - pos_start);
            L = stoi(L_str); // Convert substring to integer
        }

        start = chrono::high_resolution_clock::now();
        graph = load_graph_from_binary(temp_graph_filename);
        end = chrono::high_resolution_clock::now();
        load_graph_duration = end- start;
        cout << "Loading Graph took " << load_graph_duration.count() << " seconds.\n" << endl;

        start = chrono::high_resolution_clock::now();
        st_f = load_map_from_binary(temp_map_filename, graph);
        end = chrono::high_resolution_clock::now();
        load_map_duration = end- start;
        cout << "Loading Map took " << load_map_duration.count() << " seconds.\n" << endl;
    }

    // Make set_F
    if (argc == 6) {
        for (const auto& pair : graph.getAdjList()) {
            set_F.insert(pair.second->getLabel());
        }
    }

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
    if (argc != 6) {
        cout << "\t- L: " << L << "\n";
        cout << "\t- R: " << R << "\n";
        cout << "\t- a: " << a << "\n";
        cout << "\t- t: " << t << "\n";
        cout << "\t- Base file: " << base_filename << "\n";
    } else {
        cout << "\t- Graph file: " << graph_filename << "\n";
        cout << "\t- Map file: " << map_filename << "\n";
    }
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
    if (argc != 6) {
        cout << "\t- Base dataset load time: " << base_f_duration.count() << " seconds.\n";
    }
    cout << "\t- Query dataset load time: " << query_f_duration.count() << " seconds.\n";
    cout << "\t- Groundtruth dataset load time: " << ground_truth_duration.count() << " seconds.\n";
    if (argc != 6) {
        cout << "\t- Index build time (FilteredVamana): " << filtered_vamana_duration.count() << " seconds or " << filtered_vamana_duration.count() / 60 << " minutes.\n";
    } else {
        cout << "\t- Graph loading time: " << load_graph_duration.count() << " seconds.\n";
        cout << "\t- Map loading time: " << load_map_duration.count() << " seconds.\n";
    }
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

    cout << "Bye from filtered_main!" << endl;

    return 0;
}