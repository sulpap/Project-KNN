#include "../include/graph.hpp"
#include <fstream>
#include <cstdint>

// --Format of the Binary File for Graph:--
// Graph ID | number of nodes N (uint32_t)
// For each node:
// 	Node ID (int)
//  Node's Graph ID (int)
// 	Coordinates (vector of float --> array of floats)
// 	Label (int)
// 	Number of neighbors M (uint32_t)
// 	IDs of neighbors (list of int)

// example:
// ID_of_graph N
// ID_of_1st_node | ID_of_graph(node) | d1 | d2 | ... | d100 | C | M | id1 | id2 | ... | idM |
// ID_of_2nt_node | ID_of_graph(node) |d1 | d2 | ... | d100 | C | M | id1 | id2 | ... | idM |
// ...
// ID_of_N_node | ID_of_graph(node) | d1 | d2 | ... | d100 | C | M | id1 | id2 | ... | idM |

using namespace std;

void save_graph_to_binary(const Graph &graph, const string &filename)
{
    string path = "datasets/smallscale/" + filename;

    ofstream outFile(path, ios::binary);
    if (!outFile.is_open())
    {
        cerr << "Error opening file for writing: " << filename << endl;
        return;
    }

    // write graphId of the graph
    int graphId = graph.getGraphId();
    outFile.write(reinterpret_cast<const char *>(&graphId), sizeof(graphId));

    // write number of nodes
    int nodeCount = graph.getNodeCount();
    outFile.write(reinterpret_cast<const char *>(&nodeCount), sizeof(nodeCount));

    // write the nodes
    for (const auto &[id, nodePtr] : graph.getAdjList())
    {
        // write their ids
        int idd = nodePtr->getId();
        outFile.write(reinterpret_cast<const char *>(&idd), sizeof(idd));

        // write graphId of the node
        int graphId = nodePtr->getGraphId();
        outFile.write(reinterpret_cast<const char *>(&graphId), sizeof(graphId));

        // write the coordinates
        vector<double> coordinates = nodePtr->getCoordinates();
        int coordSize = coordinates.size();
        outFile.write(reinterpret_cast<const char *>(&coordSize), sizeof(coordSize));
        outFile.write(reinterpret_cast<const char *>(coordinates.data()), coordSize * sizeof(double));

        // write label
        int label = nodePtr->getLabel();
        outFile.write(reinterpret_cast<const char *>(&label), sizeof(label));

        // write the edges
        vector<int> neighbors = nodePtr->getNeighbors();
        int neighborCount = neighbors.size();
        outFile.write(reinterpret_cast<const char *>(&neighborCount), sizeof(neighborCount));
        for (int neighborId : neighbors)
        {
            outFile.write(reinterpret_cast<const char *>(&neighborId), sizeof(neighborId));
        }
    }

    outFile.close();
}

Graph load_graph_from_binary(const string &filename)
{
    string path = "datasets/smallscale/" + filename;

    ifstream inFile(path, ios::binary);
    if (!inFile.is_open())
    {
        cerr << "Error opening file for reading: " << filename << endl;
        throw runtime_error("File not found or cannot be opened");
    }

    Graph graph;

    // retrieve the graphId
    int graphId;
    inFile.read(reinterpret_cast<char *>(&graphId), sizeof(graphId));
    graph.setGraphId(graphId);

    // retrieve number of nodes
    int nodeCount;
    inFile.read(reinterpret_cast<char *>(&nodeCount), sizeof(nodeCount));

    // store neighbor relationships temporarily
    map<int, vector<int>> neighborMap;

    for (int i = 0; i < nodeCount; ++i)
    {
        // read node attributes
        int id;
        inFile.read(reinterpret_cast<char *>(&id), sizeof(id));

        int nodeGraphId;
        inFile.read(reinterpret_cast<char *>(&nodeGraphId), sizeof(nodeGraphId));

        int coordSize;
        inFile.read(reinterpret_cast<char *>(&coordSize), sizeof(coordSize));
        vector<double> coordinates(coordSize);
        inFile.read(reinterpret_cast<char *>(coordinates.data()), coordSize * sizeof(double));

        int label;
        inFile.read(reinterpret_cast<char *>(&label), sizeof(label));

        int neighborCount;
        inFile.read(reinterpret_cast<char *>(&neighborCount), sizeof(neighborCount));

        // read neighbor IDs and store in the map
        vector<int> neighbors(neighborCount);
        for (int j = 0; j < neighborCount; ++j)
        {
            int neighborId;
            inFile.read(reinterpret_cast<char *>(&neighborId), sizeof(neighborId));
            neighbors[j] = neighborId;
        }
        neighborMap[id] = neighbors;

        // create and add the node to the graph
        Node *node = new Node(id, coordinates, {}, label);
        node->setGraphId(nodeGraphId);
        graph.addNode(node);
    }

    // reconstruct edges using the neighbor map
    for (const auto &[id, neighbors] : neighborMap)
    {
        Node *node = graph.getNode(id);
        for (int neighborId : neighbors)
        {
            Node *neighborNode = graph.getNode(neighborId);
            if (neighborNode)
            {
                node->addEdge(neighborNode);
            }
        }
    }

    inFile.close();
    return graph;
}

// --Format of the Binary File for Map M:--
//  number of nodes in M
//  For each node:
//      Label (int) Node ID (int)

void save_map_to_binary(const map<int, Node *> &M, const string &filename)
{
    string path = "datasets/smallscale/" + filename;

    ofstream outFile(path, ios::binary);
    if (!outFile.is_open())
    {
        cerr << "Error opening file for writing: " << filename << endl;
        return;
    }

    // write the number of entries in the map
    int mapSize = M.size();
    outFile.write(reinterpret_cast<const char *>(&mapSize), sizeof(mapSize));

    // write each key-value pair (Label, Node ID)
    for (const auto &[label, nodePtr] : M)
    {
        outFile.write(reinterpret_cast<const char *>(&label), sizeof(label));

        int nodeId = nodePtr->getId();
        outFile.write(reinterpret_cast<const char *>(&nodeId), sizeof(nodeId));
    }

    outFile.close();
}

map<int, Node *> load_map_from_binary(const string &filename, const Graph &graph)
{
    string path = "datasets/smallscale/" + filename;

    ifstream inFile(path, ios::binary);
    if (!inFile.is_open())
    {
        cerr << "Error opening file for reading: " << filename << endl;
        throw runtime_error("File not found or cannot be opened");
    }

    map<int, Node *> M;

    // read the number of entries in the map
    int mapSize;
    inFile.read(reinterpret_cast<char *>(&mapSize), sizeof(mapSize));

    for (int i = 0; i < mapSize; ++i)
    {
        // retrieve the label
        int label;
        inFile.read(reinterpret_cast<char *>(&label), sizeof(label));

        // retrieve the Node ID
        int nodeId;
        inFile.read(reinterpret_cast<char *>(&nodeId), sizeof(nodeId));

        // retrieve the Node* from the graph using the Node ID
        Node *nodePtr = graph.getNode(nodeId);
        if (nodePtr == nullptr)
        {
            cerr << "Error: Node ID " << nodeId << " not found in graph." << endl;
            throw runtime_error("Node ID not found in graph");
        }

        // insert into the map to rebuild it
        M[label] = nodePtr;
    }

    inFile.close();
    return M;
}