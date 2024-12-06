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
        int id;     // starts from 0
        int graphId;
        vector<double> coordinates;
        list<Node*> edges;
        int label;

    public:
        // Constructor
        Node(int id, vector<double> coordinates, list<Node*> edges, int label = -1);

        // Copy constructor
        Node(const Node& other);

        // Move constructor
        Node(Node&& other) noexcept;

        // Destructor
        ~Node();

        // Move assignment operator
        Node& operator=(Node&& other) noexcept;

        // Getters
        int getId() const;
        int getGraphId() const;
        vector<double> getCoordinates() const;
        list<Node*> getEdges() const;
        int getLabel() const;

        // Setters
        void setId(int id);
        void setGraphId(int id);
        void setCoordinates(vector<double> coordinates);
        void setEdges(list<Node*>);
        void setLabel(int newLabel);

        // Extra Functions
        void addEdge(Node* to);
        void addCoordinate(vector<double> coordinates);
        // void addLabel

        bool edgeExists(int id); // Checks if node has an outgoing edge to the node with that id
        // bool labelExist

        double getSpecificCoordinate(int dimension);
        void setSpecificCoordinate(int dimension, double value);

        void deleteEdge(int id);

        vector<int> getNeighbors();
};

class Graph {
    private:
        int graphId;        // starts from 1. 0 is for when a node doesn't belong to any graph.
        static int currentGraphId;
        map<int, Node*> adjList;

    public:
        Graph();
        Graph(map<int, Node*> adj_list);

        ~Graph();
        void clear();

        // Getters
        int getGraphId() const;
        static int getCurrentGraphId();
        map<int, Node*> getAdjList() const;

        // Setters
        void setGraphId(int id);
        void setAdjList(map<int, Node*> adjList);

        // Extra Functions
        void addNode(Node* node);
        void moveNode(Node* node);
        Node* getNode(int id) const;
        void deleteNode(int id); // Deletes the Node entirely (Καλύτερα να χρησιμοποιείται αν το node ανήκει μόνο σε έναν γράφο)
        Node* removeNode(int id); // Removes the Node from the graph
        int getNodeCount() const;

        bool isEmpty() const;
        void addEdge(int idFrom, Node* node);
        void addEdge(int idFrom, int idTo);  // use this when 2 nodes are already in the graph, else: cout error message
        void removeEdge(int idFrom, int idTo);
        void printEdges();
        vector<int> findNodesWithLabel(int label);
        // void graphUnion(Graph& otherGraph);
        void graphUnion(Graph&& otherGraph);
        Graph graphDifference(Graph& otherGraph);
        void graphIntersection(Graph& otherGraph);
};

#endif