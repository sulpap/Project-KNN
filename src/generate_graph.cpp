#include <../include/utility.hpp>
#include "../include/graph.hpp"
#include <cstdlib>
#include <ctime>
#include <random>
#include <chrono>
#include <algorithm>

void generate_graph(Graph &graph, vector<vector<double>> &coords, int R)
{
    srand(time(0));

    // nodeIds start from 0
    for (size_t i = 0; i < coords.size(); i++) 
    {
        Node* newNode = new Node(i, coords[i], {}, 0);
        graph.addNode(newNode);
    }

    // each node has exactly R edges
    for (size_t i = 0; i < coords.size(); ++i) 
    {
        Node* current = graph.getNode(i);

        int edgesAdded = 0;

        while (edgesAdded < R) 
        {
            size_t randomNeighborId = rand() % coords.size();

            // add edge only if it's not a self-loop and doesn't already exist
            if (randomNeighborId != i && !current->edgeExists(randomNeighborId)) {
                graph.addEdge(i, randomNeighborId);
                edgesAdded++;
            }
        }
    }
}

// F is the set of existing labels
// this function generates nodes with random labels from F and adds 2 or less random edges to nodes of the same label
// --> must be at least 2 of the same label, or else one node will have no edges.
void generate_label_based_graph(Graph &graph, vector<vector<double>> &coords, const set<int> &F)
{
    srand(time(0));
    
    // convert the set of labels into a vector ------------------------- IS F A SET OF INTS OR VECTOR ?? TODO
    vector<int> labels(F.begin(), F.end());

    // make sure labels are enough for the nodes
    if (labels.size() <= 0 || coords.size() <= 0)
    {
        cerr << "Error: Not enough labels or coords." << endl;
        return;
    }

    // assign labels and create nodes
    for (size_t i = 0; i < coords.size(); ++i)
    {
        int label = labels[i % labels.size()]; // assign labels cyclically
        Node* newNode = new Node(i, coords[i], {}, label);
        graph.addNode(newNode);
    }

    // create edges such that each node has exactly 2 edges (not R) to nodes with the same label
    for (size_t i = 0; i < coords.size(); ++i)
    {
        Node* current = graph.getNode(i);
        int currentLabel = current->getLabel();

        // Find potential neighbors with the same label
        vector<int> sameLabelNodes;
        for (size_t j = 0; j < coords.size(); ++j)
        {
            if (j != i && graph.getNode(j)->getLabel() == currentLabel)
            {
                sameLabelNodes.push_back(j);
            }
        }

        // add edges based on the number of nodes available
        if (sameLabelNodes.size() >= 2)
        {
            // add edges to two random nodes
            unsigned seed = chrono::system_clock::now().time_since_epoch().count();
            shuffle(sameLabelNodes.begin(), sameLabelNodes.end(), default_random_engine(seed));
            graph.addEdge(i, sameLabelNodes[0]);
            graph.addEdge(i, sameLabelNodes[1]);
        }
        else if (sameLabelNodes.size() == 1)
        {
            // add an edge to the only available node
            graph.addEdge(i, sameLabelNodes[0]);
        }
        else
        {
            cerr << "Warning: Node " << i << " with label " << currentLabel
                 << " has no other nodes with the same label." << endl;
        }
    }
}