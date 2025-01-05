#include "../include/graph.hpp"
#include "../include/generate_graph.hpp"
#include <cstdlib>
#include <ctime>
#include <random>
#include <chrono>
#include <algorithm>

#include <unistd.h>

// creates a graph of nodes with label f
void generate_graph(Graph &graph, vector<Node *> &coords, int R)
{
    srand(time(0));

    size_t n = coords.size(); // number of points in the current graph
    if (n == 0)
        return; // avoid processing empty graphs

    // nodeIds start from 0
    for (size_t i = 0; i < coords.size(); i++) {
        graph.addNode(coords[i]);
    }

    int adjusted_R = min(static_cast<int>(n) - 1, R); // adjust R if necessary

    // each node has exactly R edges
    for (size_t i = 0; i < coords.size(); ++i)
    {
        Node *current = graph.getNode(coords[i]->getId());

        if (!current)
        {
            cout << "Node with id: " << coords[i]->getId() << " does not exist in the graph.\n";
            continue;
        }

        int edgesAdded = 0;

        set<size_t> potentialNeighbors;

        for (size_t j = 0; j < n; ++j)
        {
            if (j != i)
            {
                potentialNeighbors.insert(coords[j]->getId()); // add other nodes as potential neighbors
            }
        }

        while (edgesAdded < adjusted_R && !potentialNeighbors.empty())
        {
            auto it = potentialNeighbors.begin();
            advance(it, rand() % potentialNeighbors.size()); // select a random neighbor

            size_t randomNeighborId = *it;

            if (!current->edgeExists(randomNeighborId))
            {
                graph.addEdge(coords[i]->getId(), randomNeighborId);
                edgesAdded++;
            }
            potentialNeighbors.erase(it); // remove the selected neighbor to avoid cycles!
        }

        if (edgesAdded < adjusted_R)
        {
            printf("Warning: Node %ld could only form %d edges.\n", i, edgesAdded);
        }
    }
}

// function to randomly connect nodes to nodes with the same label
void generate_label_based_graph(Graph &graph, const vector<vector<double>> &coords) {
    srand(time(0));

    size_t n = coords.size(); // number of points in the input
    if (n == 0) {
        cerr << "Error: No coordinates provided." << endl;
        return;
    }

    map<int, vector<int>> labelToNodes; // this is to group nodes by label

    // group nodes by label (nodes are already added by the initialization function)
    for (size_t i = 0; i < n; ++i) {
        int label = static_cast<int>(coords[i][0]); // The label is the 0th element of the coords
        labelToNodes[label].push_back(i);
    }

    // add edges --> max 1 edge to a node of the same label
    for (size_t i = 0; i < n; ++i) {
        Node *current = graph.getNode(i);
        if (!current) {
            // cerr << "Error: Node with id " << i << " does not exist in the graph." << endl;
            continue;
        }

        int label = current->getLabel();
        const auto &sameLabelNodes = labelToNodes[label];

        // if no other nodes with the same label exist, skip
        if (sameLabelNodes.size() <= 1) {
            // cerr << "Warning: Node " << i << " has no other nodes with the same label to connect to." << endl;
            continue;
        }

        // select a random neighbor with the same label
        vector<int> potentialNeighbors;
        for (int neighborId : sameLabelNodes) {
            if (neighborId != i) {
                potentialNeighbors.push_back(neighborId);
            }
        }

        if (!potentialNeighbors.empty()) {
            unsigned seed = chrono::system_clock::now().time_since_epoch().count();
            shuffle(potentialNeighbors.begin(), potentialNeighbors.end(), default_random_engine(seed));
            int randomNeighborId = potentialNeighbors.front();

            if (!current->edgeExists(randomNeighborId)) {
                graph.addEdge(i, randomNeighborId);
            }
        }
    }
}
