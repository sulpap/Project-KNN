#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <set>

using namespace std;

class Node {
    private:
        int id;     // starts from 1
        int graphId;
        vector<double> coordinates;
        list<Node*> edges;

    public:
        // Constructor
        Node(int id, vector<double> coordinates, list<Node*> edges);

        // Copy constructor
        Node(const Node& other);

        // Getters
        int getId();
        int getGraphId();
        vector<double> getCoordinates();
        list<Node*> getEdges();

        // Setters
        void setId(int id);
        void setGraphId(int id);
        void setCoordinates(vector<double> coordinates);
        void setEdges(list<Node*>);

        // Extra Functions
        void addEdge(Node* to);
        void addCoordinate(vector<double> coordinates);

        bool edgeExists(int id); // Checks if node has an outgoing edge to the node with that id

        double getSpecificCoordinate(int dimension);
        void setSpecificCoordinate(int dimension, double value);

        void deleteEdge(int id);
};

class Graph {
    private:
        int graphId;        // starts from 1. 0 is for when a node doesn't belong to any graph.
        static int currentGraphId;
        map<int, Node*> adjList;

    public:
        Graph();
        Graph(map<int, Node*> adj_list);

        // Getters
        int getGraphId();
        map<int, Node*> getAdjList();

        // Setters
        void setGraphId(int id);
        void setAdjList(map<int, Node*> adjList);

        // Extra Functions
        void addNode(Node* node);
        Node* getNode(int id);
        void deleteNode(int id); // Deletes the Node entirely (Καλύτερα να χρησιμοποιείται αν το node ανήκει μόνο σε έναν γράφο)
        Node* removeNode(int id); // Removes the Node from the graph
        int getNodeCount();

        void addEdge(int idFrom, Node* node);
        void addEdge(int idFrom, int idTo);  // Αυτήν την χρησιμοποιούμε όταν είναι και τα 2 nodes ήδη μέσα στο graph, αλλιώς κάνει cout error message
        void removeEdge(int idFrom, int idTo);
        void printEdges();
        void graphUnion(Graph& otherGraph);
};

#endif