#include "../include/graph.hpp"
#include "../include/utility.hpp"
#include "../include/fvecs_read.hpp"
#include "../include/ivecs_read.hpp"
#include "../include/vamana.hpp"
#include "../include/greedysearch.hpp"
#include <cassert>
#include <algorithm>        // due to use of find()
#include <iostream>

#include <chrono>

using namespace std;

int main(int argc, char* argv[]) {
    auto total_start = chrono::high_resolution_clock::now();

    if (argc != 8) {
        cout << "Usage: " << argv[0] << " <k> <L> <R> <a> <base_file_path> <queries_file_path> <groundtruth_file_path> " << endl;
        cout << "Note: k must be an int\n";
        cout << "      L must be an int\n";
        cout << "      R must be an int\n";
        cout << "      a must be a double" << endl;
        return 1;
    }

    chrono::duration<double> base_f_duration;
    chrono::duration<double> query_f_duration;
    chrono::duration<double> ground_truth_duration;
    chrono::duration<double> vamana_duration;
    chrono::duration<double> average_query_greedy_duration;
    chrono::duration<double> total_query_greedy_duration = chrono::duration<double>::zero();
    
    // 1. Get Inputs
    int k = stoi(argv[1]);
    int L = stoi(argv[2]);
    int R = stoi(argv[3]);
    double a = stod(argv[4]);
    const char* base_filename = argv[5];
    const char* query_filename = argv[6];
    const char* groudtruth_filename = argv[7];

    cout << "Arguments:\n";
    cout << "\t- k: " << k << "\n";
    cout << "\t- L: " << L << "\n";
    cout << "\t- R: " << R << "\n";
    cout << "\t- a: " << a << "\n";
    cout << "\t- Base file: " << base_filename << "\n";
    cout << "\t- Query file: " << query_filename << "\n";
    cout << "\t- Groudtruth file: " << groudtruth_filename << endl;

    // 2. Read Files
    // Base File
    cout << "Loading Base dataset..." << endl;
    auto start = chrono::high_resolution_clock::now();
    vector<vector<float>> base_f = fvecs_read(base_filename);
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
    vector<vector<float>> query_f = fvecs_read(query_filename);
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
    vector<vector<int>> ground_truth = ivecs_read(groudtruth_filename);
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

    // 3. Call Vamana.
    Graph graph;
    cout << "Calling Vamana..." << endl;
    start = chrono::high_resolution_clock::now();
    int medoid_id = Vamana(graph, base, R, a, L);
    end = chrono::high_resolution_clock::now();
    vamana_duration = end - start;
    cout << "Vamana took " << vamana_duration.count() << " seconds." << endl;

    Node* medoid = graph.getNode(medoid_id);
    cout << "Medoid Node: \n";
    cout << "\t- Id: " << medoid->getId() << "\n";
    cout << "\t- GraphId: " << medoid->getGraphId() << "\n";
    cout << "\t- Coordinates:";
    for (double i: medoid->getCoordinates()) {
        cout << " " << i;
    }
    cout << endl;

    // 4. Call Greedy for every query.
    int totalFound = 0;
    int totalK = 0;
    float totalPercent = 0;

    for(int i = 0; i < static_cast<int>(queries.size()); i++) {
        vector<double> query = queries[i];
        set<Node*> L_set;
        set<Node*> V_set;

        cout << "Calling GreedySearch for " << i << "th query..." << endl;
        start = chrono::high_resolution_clock::now();
        GreedySearch(medoid, query, k, L, L_set, V_set);
        end = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = end - start;
        total_query_greedy_duration += duration;
        cout << "GreedySearch for " << i << "th query took " << duration.count() << " seconds." << endl;

        // 6. Compare greedy with ground truth
        vector<int> gt_sol = ground_truth[i];
        if (k > static_cast<int>(gt_sol.size())) {
            cout << "ERROR: For query (zero based) #"<< i << " can't compare greedy vs ground truth. Could compare with k up to " << gt_sol.size() << endl;
            return EXIT_FAILURE;
        }
        // we only need the top k results of ground truth to compare
        vector<int> gt(gt_sol.begin(), gt_sol.begin() + k);

        // We traverse L_set
        int found = 0;
        for (auto node : L_set) {
            int id_L = node->getId();
            if (find(gt.begin(), gt.end(), id_L) != gt.end()) {
                found++;
            }
        }

        float percent = (100 * found) / (float)k;
        cout << "Query (zero based) #" << i << " had " << percent << "% recall." << endl;
    
        // Accumulate totals for overall and average recall
        totalFound += found;
        totalK += k;
        totalPercent += percent;
    }

    // Calculate total recall across all queries
    cout << "Recalls:\n";

    float totalRecall = (100 * totalFound) / static_cast<float>(totalK);
    cout << "\t- Total recall across all queries (totalFound/totalK): " << totalRecall << "%" << endl;

    float averageRecall = totalPercent / queries.size();
    cout << "\t- Average recall across all queries (totalPercent/number_of_queries): " << averageRecall << "%" << endl;

    // 5. Print Summary
    average_query_greedy_duration = total_query_greedy_duration / queries.size();

    cout << "Timing Summary:\n";
    cout << "\t- Base dataset load time: " << base_f_duration.count() << "seconds.\n";
    cout << "\t- Query dataset load time: " << query_f_duration.count() << "seconds.\n";
    cout << "\t- Groundtruth dataset load time: " << ground_truth_duration.count() << "seconds.\n";
    cout << "\t- Index build time (Vamana): " << vamana_duration.count() << "seconds.\n";
    cout << "\t- Average time GreadySearch ran for queries: " << average_query_greedy_duration.count() << "seconds.\n";
    
    // 6. Clean up
    cout << "Cleaning..." << endl;
    graph.clear();      // once done

    auto total_end = chrono::high_resolution_clock::now();
    chrono::duration<double> total_duration = total_end - total_start;
    cout << "\nProgram ran for " << total_duration.count() << " seconds or " << total_duration.count() / 60 << " minutes.\n" << endl;

    cout << "Bye from main" << endl;

    return 0;
}

/*
[0] Examples of execution (Must be in the directory that Makefile is):
./bin/main 20 40 80 1.2 datasets/siftsmall/siftsmall_base.fvecs datasets/siftsmall/siftsmall_query.fvecs datasets/siftsmall/siftsmall_groundtruth.ivecs

./bin/main 100 200 60 1.2 datasets/siftsmall/siftsmall_base.fvecs datasets/siftsmall/siftsmall_query.fvecs datasets/siftsmall/siftsmall_groundtruth.ivecs

*/