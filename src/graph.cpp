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
    for (auto it = this->edges.begin(); it != this->edges.end(); ) {
        if ((*it)->getId() == id) {
            it = this->edges.erase(it); // erase returns the next iterator
        } else {
            ++it;
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

// void Graph::removeEdge(int idFrom, int idTo) {
//     this->adjList[idFrom]->deleteEdge(idTo);
// }

void Graph::removeEdge(int idFrom, int idTo) {
    // check if it exists
    Node* fromNode = getNode(idFrom);
    Node* toNode = getNode(idTo);
    if (!fromNode) {
        cout << "from node doesn't exist." << endl;
    } else if(!toNode) {
        cout << "destination node doesn't exist." << endl;
    } else {
        fromNode->deleteEdge(idTo);
    }
}

void Graph::printEdges() {
    cout << "Nodes & edges:" << endl;

    for (const auto& nodePair : this->adjList) {
        Node* node = nodePair.second;
        cout << "Node " << node->getId() << " has edges directed to: ";

        if (node->getEdges().empty()) {
            cout << "-";
        } else {
            for (Node* edge : node->getEdges()) {
                cout << edge->getId() << " ";
            }
        }
        cout << endl;
    }
}