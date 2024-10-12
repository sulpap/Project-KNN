#include <../include/header.hpp>

Node::Node(int id, vector<double> coordinates, list<Node*> edges)
    : id{ id }
    , coordinates{ coordinates }
    , edges{ edges } {

}

int Node::getId() {
    return this->id;
}

vector<double> Node::getCoordinates() {
    return this->coordinates;
}

list<Node*> Node::getEdges() {
    return this->edges;
}

void Node::setId(int id) {
    this->id = id;
}

// TODO maybe needs change
void Node::setCoordinates(vector<double> coordinates) {
    this->coordinates = coordinates;
}

// TODO maybe needs change
void Node::setEdges(list<Node*> edges) {
    this->edges = edges;
}

void Node::addEdge(Node* to) {
    this->edges.push_back(to);
}

void Node::addCoordinate(vector<double> coordinate) {
    this->coordinates.insert(this->coordinates.end(), coordinates.begin(), coordinates.end());
}

bool Node::edgeExists(int id) {
    for (Node* edge: this->edges) {
        if (edge->getId() == id) {
            return true;
        }
    }
    return false;
}

double Node::getSpecificCoordinate(int dimension) {
    return this->coordinates.at(dimension);
}

void Node::setSpecificCoordinate(int dimension, double value) {
    this->coordinates[dimension] = value;
}

void Node::deleteEdge(int id) {
    for (Node* edge: this->edges) {
        if (edge->getId() == id) {
            this->edges.remove(edge);
        }
    }
}





Graph::Graph() {

}

// TODO maybe needs change
Graph::Graph(map<int, Node*> adj_list) {
    this->adjList = adj_list;
}

map<int, Node*> Graph::getAdjList() {
    return this->adjList;
}

// TODO maybe needs change
void Graph::setAdjList(map<int, Node*> adjList) {
    this->adjList = adjList;
}

void Graph::addNode(Node* node) {
    this->adjList[node->getId()] = node;
    // Other implementation
    // this->adjList.insert(pair<int, Node*>(node->getId(), node));
}

Node* Graph::getNode(int id) {
    return this->adjList[id];
}

void Graph::deleteNode(int id) {
    this->adjList.erase(id);
    // Maybe needs to delete the node manually
}

void Graph::addEdge(int idFrom, Node* node) {
    this->adjList[idFrom]->addEdge(node);
}

void Graph::removeEdge(int idFrom, int idTo) {
    this->adjList[idFrom]->deleteEdge(idTo);
}