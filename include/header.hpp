#ifndef HEADER_HPP
#define HEADER_HPP

#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <set>

using namespace std;

class Node {
    private:
        int id;
        vector<double> coordinates;
        list<Node*> edges;

    public:
        // Constructor
        Node(int id, vector<double> coordinates, list<Node*> edges);

        // Getters
        int getId();
        vector<double> getCoordinates();
        list<Node*> getEdges();

        // Setters
        void setId(int id);
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
        map<int, Node*> adjList;

    public:
        Graph();
        Graph(map<int, Node*> adj_list);

        // Getters
        map<int, Node*> getAdjList();

        // Setters
        void setAdjList(map<int, Node*> adjList);

        // Extra Functions
        void addNode(Node* node);
        Node* getNode(int id);
        void deleteNode(int id);

        void addEdge(int idFrom, Node* node);
        // void addEdge(int idFrom, int idTo); // Δεν μπορούμε να το φτιάξουμε αυτό διότι έτσι όπως το έχουμε, το node περιέχει ένα list από nodes για edges και όχι ids.
        void removeEdge(int idFrom, int idTo);
        void printEdges();
};

#endif