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

