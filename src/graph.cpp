#include <../include/graph.hpp>

Node::Node(int id, vector<double> coordinates, list<Node*> edges)
    : id{ id }
    , graphId{ 0 }
    , coordinates{ coordinates }
    , edges{ edges } {

}

// Copy constructor
Node::Node(const Node& other)
    : id(other.id)
    , graphId(other.graphId)
    , coordinates(other.coordinates)
    , edges(other.edges) {

}

Node::~Node() {}

int Node::getId() {
    return this->id;
}

int Node::getGraphId() {
    return this->graphId;
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

void Node::setGraphId(int id) {
    this->graphId = id;
}

void Node::setCoordinates(vector<double> coordinates) {
    this->coordinates = coordinates;
}

void Node::setEdges(list<Node*> edges) {
    this->edges = edges;
}

void Node::addEdge(Node* to) {
    if (!edgeExists(to->getId())) {
        this->edges.push_back(to);
    }
}

void Node::addCoordinate(vector<double> coordinate) {
    this->coordinates.insert(this->coordinates.end(), coordinate.begin(), coordinate.end());
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
    if (dimension < 0 || static_cast<size_t>(dimension) >= this->coordinates.size()) {
        throw out_of_range("Invalid dimension");
    }
    return this->coordinates[dimension];

}

void Node::setSpecificCoordinate(int dimension, double value) {
    if (dimension < 0 || static_cast<size_t>(dimension) >= this->coordinates.size()) {
        throw out_of_range("Invalid dimension");
    }
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

vector<int> Node::getNeighbors() 
{
    vector<int> neighbors;
    for (Node* neighbor : this->edges) {
        neighbors.push_back(neighbor->getId());
    }
    return neighbors;
}


// // Move constructor
// Node::Node(Node&& other) noexcept 
//     : id(other.id), coordinates(std::move(other.coordinates)), edges(std::move(other.edges)) 
// {
//     // 'other' is left in a valid, but empty state
//     other.id = 0;
// }


// // Move assignment operator
// Node& Node::operator=(Node&& other) noexcept {
//     if (this != &other) {  // Avoid self-assignment
//         // Clear the previous data of this
//         coordinates.clear();
//         edges.clear();

//         // Transfer ownership of resources from 'other' to 'this'
//         id = other.id;
//         coordinates = std::move(other.coordinates);
//         edges = std::move(other.edges);

//         // Leave 'other' in a valid but empty state
//         other.id = 0;
//     }
//     return *this;
// }


int Graph::currentGraphId = 1;

Graph::Graph() : graphId(currentGraphId++) {}

Graph::Graph(map<int, Node*> adj_list) : graphId(currentGraphId++) {
    this->adjList = adj_list;
}

void Graph::clear() {
    for (auto& nodePair : adjList) {
        delete nodePair.second;
    }
    adjList.clear();     
}

Graph::~Graph() {}

int Graph::getGraphId() {
    return this->graphId;
}

int Graph::getCurrentGraphId() {
    return currentGraphId;
}

map<int, Node*> Graph::getAdjList() {
    return this->adjList;
}

void Graph::setGraphId(int id) {
    this->graphId = id;
}

void Graph::setAdjList(map<int, Node*> adjList) {
    this->adjList = adjList;
}

void Graph::addNode(Node* node) {
    node->setGraphId(this->getGraphId());
    this->adjList[node->getId()] = node;
}

Node* Graph::getNode(int id) {
    if (this->adjList.find(id) != this->adjList.end()) {
        return this->adjList[id];
    }
    return nullptr;  // Node with the given id doesn't exist
}

void Graph::deleteNode(int id) {
    if (this->adjList.find(id) != this->adjList.end()) {
        for (auto& pair : adjList) {
            pair.second->deleteEdge(id);  // Remove any edge to the node being deleted
        }
        delete this->adjList[id];
        this->adjList.erase(id);
    }
}

Node* Graph::removeNode(int id) {
    if (this->adjList.find(id) != this->adjList.end()) {
        for (auto& pair : adjList) {
            pair.second->deleteEdge(id);  // Remove any edge to the node being deleted
        }
        Node* temp = adjList[id];
        this->adjList.erase(id);
        temp->setGraphId(0);
        return temp;
    } else {
        return nullptr;
    }
}

int Graph::getNodeCount() {
    return this->adjList.size();
}


void Graph::addEdge(int idFrom, Node* node) {
    this->adjList[idFrom]->addEdge(node);
}

void Graph::addEdge(int idFrom, int idTo) {
    if (this->adjList.find(idFrom) != this->adjList.end()) {
        if (this->adjList.find(idTo) != this->adjList.end()) {
            this->adjList[idFrom]->addEdge(this->adjList[idTo]);
        } else {
            cout << "To node doesn't exist in the graph" << endl;
        }
    } else {
            cout << "From node doesn't exist in the graph" << endl;
    }
}

void Graph::removeEdge(int idFrom, int idTo) 
{
    // check if it exists
    Node* fromNode = getNode(idFrom);
    Node* toNode = getNode(idTo);
    if (fromNode == nullptr) {
        cout << "from node doesn't exist." << endl;
    } else if(toNode == nullptr) {
        cout << "destination node doesn't exist." << endl;
    } else {
        fromNode->deleteEdge(idTo);
    }
}

void Graph::printEdges() 
{
    cout << "Nodes & edges:" << endl;

    for (const auto& it : this->adjList) {
        Node* node = it.second;
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

void Graph::graphUnion(Graph& otherGraph)                                   // we basically add another graph to our graph
{
    for (auto& it : otherGraph.getAdjList()) {                              // note: & --> working with references to the elements in the container, not copies
        
        int nodeId = it.first;
        Node* otherNode = it.second;

        //check if the node already exists in the current graph
        if (this->adjList.find(nodeId) == this->adjList.end()) {            // note: an h find epistrepsei to end, tote paei na pei oti DEN vrethike
            //if it doesn't, add it
            this->addNode(new Node(*otherNode));                            // copy constructor
        }
        
        //iterate through the edges of the node from otherGraph
        for (Node* edge : otherNode->getEdges()) {
            //check if the edge already exists
            if (!this->getNode(nodeId)->edgeExists(edge->getId())) {
                //add the edge
                this->addEdge(nodeId, edge);
            }
        }
    }
}


Graph Graph::graphDifference(Graph& otherGraph) {
    Graph result(*this);  // Create a copy of the current graph (copy constructor should be defined)

    // Iterate through the nodes in the current Graph
    for (auto& it : this->getAdjList()) {
        int nodeId = it.first;

        // Check if the node exists in the other Graph
        if (otherGraph.adjList.find(nodeId) != otherGraph.adjList.end()) {  // Was found in other Graph
            result.removeNode(nodeId);      // removes also edges pointing to that node
        }
    }

    return result;  // Return the modified graph
}

void Graph::graphIntersection(Graph& otherGraph) {
    // Temporary map to store nodes for the intersection graph
    map<int, Node*> intersectAdjList;

    // Iterate over nodes in the current graph
    for (auto& [nodeId, currentNode] : this->adjList) {
        // Check if this node exists in the other graph
        if (otherGraph.getAdjList().count(nodeId) > 0) {
            Node* otherNode = otherGraph.getNode(nodeId);

            // Create a new node (copy of currentNode) for the intersection
            Node* intersectNode = new Node(*currentNode);
            intersectAdjList[nodeId] = intersectNode;

            // Retain only the common edges
            list<Node*> intersectEdges;
            for (Node* edge : currentNode->getEdges()) {
                if (otherNode->edgeExists(edge->getId())) {
                    // Add a copy of the edge node if it exists in both graphs
                    int edgeId = edge->getId();
                    if (intersectAdjList.count(edgeId) == 0) {
                        intersectAdjList[edgeId] = new Node(*otherGraph.getNode(edgeId));
                    }
                    intersectEdges.push_back(intersectAdjList[edgeId]);
                }
            }

            // Set the intersected edges for the intersectNode
            intersectNode->setEdges(intersectEdges);
        }
    }

    // Replace the current graph's adjacency list with the intersection result
    for (auto& [id, node] : this->adjList) {
        delete node;
    }
    this->adjList = intersectAdjList;
}

