#include "catch.hpp"
#include "../include/graph.hpp"
#include "../include/save_graph_to_binary.hpp"
#include <fstream>
#include <vector>

// TODO
TEST_CASE("save_graph_to_binary function test") 
{
    // example graph
    Graph graph;

    vector<double> coords1(101, 0.1);
    Node* node1 = new Node(0, coords1, {}, 1); // Node 1 with label 1

    vector<double> coords2(101, 0.2);
    Node* node2 = new Node(1, coords2, {}, 2); // Node 2 with label 2

    node1->addEdge(node2); // Add edge between node 1 and node 2
    node2->addEdge(node1); // Add edge between node 2 and node 1

    graph.addNode(node1);
    graph.addNode(node2);

    // save graph to binary file
    string file_path = "test_graph.bin";
    save_graph_to_binary(graph, file_path);

    // read the graph back from the binary file
    

    // test if the loaded graph matches the original graph
    

    // verify that the neighbors are correctly restored
  

    // clean up the file (???)
    remove(file_path.c_str());
    graph.clear();
}
