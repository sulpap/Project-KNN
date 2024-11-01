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
    //TODO we also need query node from arg

    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <fvecs file> <number of nearest neighbors to find>" << endl;
        return 1;
    }

    //"../datasets/siftsmall/siftsmall_base.fvecs";
    const char* filename = argv[1];
    int k = stoi(argv[2]);

    // ----------------------- Read the fvecs file given -------------------------

    // Read all vectors from the fvecs file
    vector<vector<float>> vectors = fvecs_read(filename);

    // verify the number of vectors read
    if (!vectors.empty()) {
        cout << "Total vectors read: " << vectors.size() << endl;
    } else {
        cout << "No vectors read from the file." << endl;
        return EXIT_FAILURE;
    }

    //print the first vector to verify
    if (!vectors.empty()) {
        cout << "First vector (" << vectors[0].size() << " dimensions): ";
        for (const auto& value : vectors[0]) {
            cout << value << " ";
        }
        cout << endl;
    } else {
        cout << "No vectors read from the file." << endl;
        return EXIT_FAILURE;
    }

    // float to double to use for nodes
    vector<vector<double>> coordinates;
    for (const auto& vec : vectors) {
        coordinates.push_back(vector<double>(vec.begin(), vec.end()));
    }

    cout << "All vectors processed successfully." << endl;

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

    vector<vector<double>> coords = {
        {1.0, 2.0},
        {2.0, 3.0},
        {3.0, 4.0},
        {4.0, 5.0},
        {2.0, 1.0}
    };

    int maxNodesEdges = 3;
    // k = 2;
    int a = 1; 

    cout << "running vamana..." << endl;
    Vamana(graph, coords, maxNodesEdges, k, a);
    
    // cout << endl << "Cleaning..." << endl;

    // delete node1;
    // delete node2;
    // delete node3;
    // delete node4;
    // delete node5;
    // delete node6;

    cout << endl << "Bye!" << endl;

    return 0;
}