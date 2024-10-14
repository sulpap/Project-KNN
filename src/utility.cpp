#include <../include/utility.hpp>
#include "../include/graph.hpp"
#include <cmath>
#include <vector>
#include <fstream>
#include <cassert>

// TODO
Graph generate_graph(const vector<vector<double>> vectors)    //gives one edge to each node?? kai ids should  be global??
{ 
    Graph graph;

    for (auto i = 0.0; i < vectors.size(); ++i) 
    {
        Node* node = new Node(i, vectors[i], list<Node*>{});
        
        graph.addNode(node);
    }
    
    // add edges

    return graph;

}

    
//TODO
void read_fvecs(const char* filename) 
{
    FILE* f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "could not open %s\n", filename);
        perror("");
        exit(EXIT_FAILURE);
    }

    // find out dimension
    int dimension;
    fread(&dimension, 1, sizeof(int), f);

    assert((dimension > 0 && dimension < 1000000) || !"unreasonable dimension");

    // https://github.com/facebookresearch/faiss/blob/main/demos/demo_sift1M.cpp

    // generate_graph

}

double euclideanDistance(vector<double> coords1, vector<double> coords2) 
{
    double sum = 0.0;

    for (auto i = 0.0; i < coords1.size(); ++i) {                                 // auto or smth else??
        sum += pow(coords2[i] - coords1[i], 2);
    }

    return sqrt(sum);
}

double calculate_eclideanDistance(Node* node1, Node* node2)
{
    //take the coords from the two nodes and call euclideanDistance
    return euclideanDistance(node1->getCoordinates(), node2->getCoordinates());
}