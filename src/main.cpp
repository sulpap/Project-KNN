#include <iostream>
#include "../include/header.hpp"

using namespace std;

int main() {

    Node* node1 = new Node(1, vector<double>{1.0, 2.0}, list<Node*>{});
    Node* node2 = new Node(2, vector<double>{2.0, 3.0}, list<Node*>{});
    Node* node3 = new Node(3, vector<double>{3.0, 4.0}, list<Node*>{});
    Node* node4 = new Node(4, vector<double>{4.0, 5.0}, list<Node*>{});

    Graph graph;

    graph.addNode(node1);
    graph.addNode(node2);
    graph.addNode(node3);
    graph.addNode(node4);

    graph.addEdge(1, node2); // 1 δείχνει στο 2 
    graph.addEdge(2, node3);
    graph.addEdge(3, node4);
    graph.addEdge(4, node2);

    graph.printEdges();

    cout << "delete edge from 1 to 2" << endl;
    graph.removeEdge(1, 2);

    graph.printEdges();

    cout << "end" << endl;

    return 0;
}