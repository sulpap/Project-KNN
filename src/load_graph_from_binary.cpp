#include "../include/graph.hpp"
#include <fstream>
#include <cstdint>

using namespace std;

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

