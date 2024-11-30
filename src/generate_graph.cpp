#include "../include/graph.hpp"
#include "../include/generate_graph.hpp"
#include <cstdlib>
#include <ctime>
#include <random>
#include <chrono>
#include <algorithm>

// this new approach of generate_graph, creates a graph of nodes with label f
void generate_graph(Graph &graph, vector<vector<double>> &coords, int R, int f, unordered_map<int, int> &indexes)
{
    srand(time(0));

    size_t n = coords.size();  // number of points in the current graph
    if (n == 0) return;  // avoid processing empty graphs

    // nodeIds start from 0. we need them to be unique for each label, so nodeIds have this form: f × offset + i
    // offset must be larger than the max number of nodes per label
    int offset = 10000;
    for (size_t i = 0; i < coords.size(); i++) 
    {
        int unique_id = f * offset + i;
        Node* newNode = new Node(unique_id, coords[i], {}, f);
        graph.addNode(newNode);

        // store id-index since they don't match
        indexes[i] = unique_id;
    }

    int adjusted_R = min(static_cast<int>(n) - 1, R);  // adjust R if necessary

    // each node has exactly R edges
    for (size_t i = 0; i < coords.size(); ++i) 
    {
        int unique_id = f * offset + i;
        Node* current = graph.getNode(unique_id);

        int edgesAdded = 0;

        set<size_t> potentialNeighbors;
        for (size_t j = 0; j < n; ++j) 
        {
            if (j != i) 
            {
                potentialNeighbors.insert(f * offset + j); // add other nodes as potential neighbors
            }
        }

        while (edgesAdded < adjusted_R && !potentialNeighbors.empty()) 
        {
            auto it = potentialNeighbors.begin();
            advance(it, rand() % potentialNeighbors.size()); // select a random neighbor ------------------------------------

            size_t randomNeighborId = *it;

            if (!current->edgeExists(randomNeighborId)) 
            {
                graph.addEdge(unique_id, randomNeighborId);
                edgesAdded++;
            }

            potentialNeighbors.erase(it);  // remove the selected neighbor to avoid cycles!
        }

        if (edgesAdded < adjusted_R) 
        {
            printf("Warning: Node %d could only form %d edges.\n", unique_id, edgesAdded);
        }
    }
    cout << "end of generate graph.\n";
}


// F is the set of existing labels
// this function generates nodes with random labels from F and adds 2 or less random edges to nodes of the same label
// --> must be at least 2 of the same label, or else one node will have no edges.
void generate_label_based_graph(Graph &graph, vector<vector<double>> &coords, const set<int> &F)
{
    srand(time(0));
    
    // convert the set of labels into a vector
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