#include "../include/graph.hpp"
#include <fstream>
#include <cstdint>

// --Format of the Binary File:--
// number of nodes N (uint32_t)
// For each node:
// 	Node ID (int)
// 	Label (int)
// 	Coordinates (vector of float --> array of floats)
// 	Number of neighbors M (uint32_t)
// 	IDs of neighbors (list of int)

// example: 
// N 
// ID_of_1st_node | C | d1 | d2 | ... | d100 | M | id1 | id2 | ... | idM |
// ID_of_2nt_node | C | d1 | d2 | ... | d100 | M | id1 | id2 | ... | idM |
// ...
// ID_of_N_node | C | d1 | d2 | ... | d100 | M | id1 | id2 | ... | idM |

using namespace std;

void save_graph_to_binary(const Graph& graph, const string& filename) {
    ofstream outFile(filename, ios::binary);
    if (!outFile.is_open()) {
        cerr << "Error opening file for writing: " << filename << endl;
        return;
    }

    // write graphId
    int graphId = graph.getGraphId();
    outFile.write(reinterpret_cast<const char*>(&graphId), sizeof(graphId));

    // write number of nodes
    int nodeCount = graph.getNodeCount();
    outFile.write(reinterpret_cast<const char*>(&nodeCount), sizeof(nodeCount));

    // write the nodes
    for (const auto& [id, nodePtr] : graph.getAdjList()) {
        int idd = nodePtr->getId();
        outFile.write(reinterpret_cast<const char*>(&idd), sizeof(idd));

        int graphId = nodePtr->getGraphId();
        outFile.write(reinterpret_cast<const char*>(&graphId), sizeof(graphId));

        // write the coordinates
        vector<double> coordinates = nodePtr->getCoordinates();
        int coordSize = coordinates.size();
        outFile.write(reinterpret_cast<const char*>(&coordSize), sizeof(coordSize));
        outFile.write(reinterpret_cast<const char*>(coordinates.data()), coordSize * sizeof(double));

        // write label
        int label = nodePtr->getLabel();
        outFile.write(reinterpret_cast<const char*>(&label), sizeof(label));

        // write the edges
        vector<int> neighbors = nodePtr->getNeighbors();
        int neighborCount = neighbors.size();
        outFile.write(reinterpret_cast<const char*>(&neighborCount), sizeof(neighborCount));
        for (int neighborId : neighbors) {
            outFile.write(reinterpret_cast<const char*>(&neighborId), sizeof(neighborId));
        }
    }

    outFile.close();
}

Graph load_graph_from_binary(const string& filename) {
    ifstream inFile(filename, ios::binary);
    if (!inFile.is_open()) {
        cerr << "Error opening file for reading: " << filename << endl;
        throw runtime_error("File not found or cannot be opened");
    }

    Graph graph;

    // retrieve the graphId
    int graphId;
    inFile.read(reinterpret_cast<char*>(&graphId), sizeof(graphId));
    graph.setGraphId(graphId);

    // retrieve number of nodes
    int nodeCount;
    inFile.read(reinterpret_cast<char*>(&nodeCount), sizeof(nodeCount));

    // store neighbor relationships temporarily
    map<int, vector<int>> neighborMap;

    for (int i = 0; i < nodeCount; ++i) {
        // read node attributes
        int id;
        inFile.read(reinterpret_cast<char*>(&id), sizeof(id));

        int nodeGraphId;
        inFile.read(reinterpret_cast<char*>(&nodeGraphId), sizeof(nodeGraphId));

        int coordSize;
        inFile.read(reinterpret_cast<char*>(&coordSize), sizeof(coordSize));
        vector<double> coordinates(coordSize);
        inFile.read(reinterpret_cast<char*>(coordinates.data()), coordSize * sizeof(double));

        int label;
        inFile.read(reinterpret_cast<char*>(&label), sizeof(label));

        int neighborCount;
        inFile.read(reinterpret_cast<char*>(&neighborCount), sizeof(neighborCount));

        // read neighbor IDs and store in the map
        vector<int> neighbors(neighborCount);
        for (int j = 0; j < neighborCount; ++j) {
            int neighborId;
            inFile.read(reinterpret_cast<char*>(&neighborId), sizeof(neighborId));
            neighbors[j] = neighborId;
        }
        neighborMap[id] = neighbors;

        // create and add the node to the graph
        Node* node = new Node(id, coordinates, {}, label);
        node->setGraphId(nodeGraphId);
        graph.addNode(node);
    }

    // reconstruct edges using the neighbor map
    for (const auto& [id, neighbors] : neighborMap) {
        Node* node = graph.getNode(id);
        for (int neighborId : neighbors) {
            Node* neighborNode = graph.getNode(neighborId);
            if (neighborNode) {
                node->addEdge(neighborNode);
            }
        }
    }

    inFile.close();
    return graph;
}