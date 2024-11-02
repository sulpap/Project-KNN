#include "../include/graph.hpp"
#include "../include/utility.hpp"
#include "../include/fvecs_read.hpp"
#include "../include/ivecs_read.hpp"
#include "../include/vamana.hpp"
// #include "../include/greedysearch.hpp"
#include "../include/experimentalgreedysearh.hpp"
#include <cassert>
#include <algorithm>        // due to use of find()
#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 8) {
        cout << "Usage: " << argv[0] << " <base file> <queries file> <ground truth file> <a> <L> <R> <k>" << endl;
        return 1;
    }

// 1. Διάβασμα base file
    const char* base_filename = argv[1];
    vector<vector<float>> base_f = fvecs_read(base_filename);
    
    // Check size of base_f
    if (!base_f.empty()) {
        cout << "Number of points (base.fvecs): " << base_f.size() << endl;
    } else {
        cout << "No base vectors read from the file." << endl;
        return EXIT_FAILURE;
    }

    vector<vector<double>> base = convert_to_double(base_f); 

// 2. Κλήση Vamana
    double a = stod(argv[4]); 
    cout << "a = " << a << endl;
    int l = stoi(argv[5]);
    cout << "l = " << l << endl;
    int r = stoi(argv[6]);
    cout << "r = " << r << endl;

    Graph graph;
    cout << "Start of Vamana call from main" << endl;
    int medoid_id = Vamana(graph, base, r, a, l);
    cout << "End of Vamana call from main" << endl;

    Node* medoid = graph.getNode(medoid_id);

// 3. Διάβασμα queries file
    const char* query_filename = argv[2];
    vector<vector<float>> query_f = fvecs_read(query_filename);
    
    // Check size of query_f
    if (!query_f.empty()) {
        cout << "Number of queries (query.fvecs): " << query_f.size() << endl;
    } else {
        cout << "No queries vectors read from the file." << endl;
        return EXIT_FAILURE;
    }

    vector<vector<double>> queries = convert_to_double(query_f); 

// 4. Διάβασμα ground truth για κάθε query
    const char* groudtruth_filename = argv[3];
    vector<vector<int>> ground_truth = ivecs_read(groudtruth_filename);

    // Check size of ground_truth
    if (!ground_truth.empty()) {
        cout << "Number of ground_truth (ground_truth.ivecs): " << ground_truth.size() << endl;
    } else {
        cout << "No ground_truth vectors read from the file." << endl;
        return EXIT_FAILURE;
    }


// 5. Κλήση greedy για κάθε query
    int k = stoi(argv[7]);
    cout << "k = " << k << endl;

    for(int i = 0; i < static_cast<int>(queries.size()); i++) {
        vector<double> query = queries[i];
        set<Node*> L_set;
        set<Node*> V_set;

        cout << "Greedy call for " << i << "th query" << endl;
        GreedySearch(graph, medoid, query, k, l, L_set, V_set);

// 6. Σύγκριση greedy με ground truth
        vector<int> gt_sol = ground_truth[i];
        if(k > static_cast<int>(gt_sol.size())) {
            cout << "For query (zero based) #"<< i << " can't compare greedy vs ground truth. Could compare with k up to " << gt_sol.size() << endl;
            return EXIT_FAILURE;
        }
        vector<int> gt(gt_sol.begin(), gt_sol.begin() + k);     // Μάς ενδιαφέρουν για σύγκριση μόνο τα top k αποτελέσματα του ground truth

        // We traverse L_set
        int found = 0;
        for (auto node : L_set) {
            int id_L = node->getId();
            if(find(gt.begin(), gt.end(), id_L) != gt.end()) {
                found++;
            }
        }
        float percent = (100 * found) / (float)k;
        cout << "Query (zero based) #" << i << " had " << percent << "% success running greedy after vamana" << endl;
    }

    graph.clear();      // once done

    cout << "Bye from main" << endl;

    return 0;
}

/*
[0] Example of execution:
./bin/main datasets/siftsmall/siftsmall_base.fvecs datasets/siftsmall/siftsmall_query.fvecs datasets/siftsmall/siftsmall_groundtruth.ivecs 1.2 10000 12 1

[1] Μέχρι να απαντήσει ο Πασκαλής στο @40 θεωρούμε ότι το a δίνεται από τη γραμμή εντολών
*/