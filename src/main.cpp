#include <iostream>
#include <cmath> 
#include <fstream>
#include "../include/graph.hpp"
#include "../include/utility.hpp"

using namespace std;


int main() {

    Node* node1 = new Node(1, vector<double>{1.0, 2.0}, list<Node*>{});
    Node* node2 = new Node(2, vector<double>{2.0, 3.0}, list<Node*>{});
    Node* node3 = new Node(3, vector<double>{3.0, 4.0}, list<Node*>{});
    Node* node4 = new Node(4, vector<double>{4.0, 5.0}, list<Node*>{});

    Graph graph1;

    graph1.addNode(node1);
    graph1.addNode(node2);
    graph1.addNode(node3);
    graph1.addNode(node4);

    cout << "distance between node1 and node2: " << calculate_eclideanDistance(node1, node2) << endl;

    graph1.addEdge(1, node2); // 1 δείχνει στο 2 
    graph1.addEdge(1, node3);
    graph1.addEdge(2, node3);
    graph1.addEdge(3, node4);
    graph1.addEdge(4, node2);

    graph1.printEdges();

    cout << "delete edge from 1 to 2" << endl;
    graph1.removeEdge(1, 2);

    graph1.printEdges();

    // ---------- second graph ------------

    Node* node5 = new Node(5, vector<double>{2.0, 1.0}, list<Node*>{});
    Node* node6 = new Node(6, vector<double>{4.0, 6.0}, list<Node*>{});

    Graph graph2;
    graph2.addNode(node5);
    graph2.addNode(node6);
    graph2.addNode(node1);

    graph2.addEdge(5, node6);
    graph2.addEdge(1, node6);

    cout << endl << "second graph:" << endl;
    graph2.printEdges();

    graph1.graphUnion(graph2);

    cout << endl << "new graph after union: " << endl;

    graph1.printEdges();
    
    cout << "end" << endl;

    return 0;
}
