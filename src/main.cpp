#include <iostream>
#include <cmath> 
#include <fstream>
#include "../include/graph.hpp"
#include "../include/utility.hpp"
#include "../include/fvecs_read.hpp"
#include "../include/generate_graph.hpp"
#include "../include/vamana.hpp"

#include <chrono>   // temp --- for time in findMedoid

using namespace std;


int main(int argc, char* argv[]) 
{
    //TODO query,ground truth

    if (argc != 5) {
        cout << "Usage: " << argv[0] << " <fvecs base file> <fvecs query file> <ground truth file> <number of nearest neighbors to find (k)>" << endl;
        return 1;
    }

    const char* base_filename = argv[1]; //"../datasets/siftsmall/siftsmall_base.fvecs";
    const char* query_filename = argv[2]; //"../datasets/siftsmall/siftsmall_query.fvecs";
    const char* groudtruth_filename = argv[3]; //"../datasets/siftsmall/siftsmall_groundtruth.fvecs";
    int k = stoi(argv[4]);

    // ----------------------- Read the base fvecs file given -------------------------

    // Read all vectors from the fvecs file
    vector<vector<float>> vectors = fvecs_read(base_filename);

    // verify the number of vectors read
    if (!vectors.empty()) {
        cout << "Total vectors read: " << vectors.size() << endl;
    } else {
        cout << "No vectors read from the file." << endl;
        return EXIT_FAILURE;
    }

    // float to double to use for nodes
    vector<vector<double>> coordinates;
    for (const auto& vec : vectors) {
        coordinates.push_back(vector<double>(vec.begin(), vec.end()));
    }

    cout << "Base vectors processed successfully." << endl;

    // ----------------------- Read the query fvecs file given -------------------------

    // same process

    vector<vector<float>> temp_queries = fvecs_read(query_filename);

    // verify the number of vectors read
    if (!temp_queries.empty()) {
        cout << "Total queries read: " << temp_queries.size() << endl;
    } else {
        cout << "No queries read from the file." << endl;
        return EXIT_FAILURE;
    }

    // float to double to use for nodes
    vector<vector<double>> queries;
    for (const auto& query : temp_queries) {
        queries.push_back(vector<double>(query.begin(), query.end()));
    }

    cout << "Query vectors processed successfully." << endl;

    // ----------------------------- findMedoid ------------------------------------

    // auto start = chrono::high_resolution_clock::now();

    // int imedoid = findMedoid(coordinates);

    // auto end = chrono::high_resolution_clock::now();
    // chrono::duration<double> duration = end - start;
    // cout << "findMedoid took: " << duration.count() << " seconds" << endl;

    // cout << "medoid index is:" << imedoid << endl;

    // cout << "medoid coordinates: ";
    // for (const auto& value : coordinates[imedoid]) {
    //     cout << value << " ";
    // }
    // cout << endl;

   // ------------------------------- vamana ----------------------------------

    Graph graph;

    // vector<vector<double>> coords = {
    //     {1.0, 2.0},
    //     {2.0, 3.0},
    //     {3.0, 4.0},
    //     {4.0, 5.0},
    //     {2.0, 1.0}
    // };

    int R = 3;
    int a = 1; // a should be float -> 1.2
    int int_L = 10000;

    cout << "running vamana..." << endl;

    auto start = chrono::high_resolution_clock::now();

    int med = Vamana(graph, coordinates, R, a, int_L);
    
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "Vamana took: " << duration.count() << " seconds" << endl;

    cout << "medoid is:" << med << endl;
    
    cout << endl << "Cleaning..." << endl;

    graph.clear();

    cout << endl << "Bye!" << endl;

    return 0;
}