#include "../include/graph.hpp"
#include "../include/generate_graph.hpp"
#include <cstdlib>
#include <ctime>
#include <random>
#include <chrono>
#include <algorithm>

// creates a graph of nodes with label f
void generate_graph(Graph &graph, vector<vector<double>> &coords, int R, int f, unordered_map<int, int> &indexes)
{
    srand(time(0));

    size_t n = coords.size(); // number of points in the current graph
    if (n == 0)
        return; // avoid processing empty graphs

    // nodeIds start from 0. we need them to be unique for each label, so ids won't overlap
    // so nodeIds have this form: f × offset + i
    // offset must be larger than the max number of nodes per label
    for (size_t i = 0; i < coords.size(); i++)
    {
        int unique_id = f * OFFSET + i;
        Node *newNode = new Node(unique_id, coords[i], {}, f);
        graph.addNode(newNode);

        // store id-index since they don't match
        indexes[i] = unique_id;
    }

    int adjusted_R = min(static_cast<int>(n) - 1, R); // adjust R if necessary

    // each node has exactly R edges
    for (size_t i = 0; i < coords.size(); ++i)
    {
        int unique_id = f * OFFSET + i;
        Node *current = graph.getNode(unique_id);

        if (!current)
        {
            cout << "Node with id: " << unique_id << " does not exist in the graph.\n";
            continue;
        }

        int edgesAdded = 0;

        set<size_t> potentialNeighbors;
        for (size_t j = 0; j < n; ++j)
        {
            if (j != i)
            {
                potentialNeighbors.insert(f * OFFSET + j); // add other nodes as potential neighbors
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

            potentialNeighbors.erase(it); // remove the selected neighbor to avoid cycles!
        }

        if (edgesAdded < adjusted_R)
        {
            printf("Warning: Node %d could only form %d edges.\n", unique_id, edgesAdded);
        }
    }
}

// F is the set of existing labels
// this function generates nodes with random labels from F and adds 2 or less random edges to nodes of the same label
// --> must be at least 2 of the same label, or else one node will have no edges.
void generate_label_based_graph(Graph &graph, vector<vector<double>> &coords, const set<int> &F)
{
    srand(time(0));

    // convert the set of labels into a vector
    vector<int> labels(F.begin(), F.end());
    // add -1 to it, to represent the nodes with no label
    labels.push_back(-1);

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
        Node *newNode = new Node(i, coords[i], {}, label);
        graph.addNode(newNode);
    }

    // create edges such that each node has exactly 2 edges (not R) to nodes with the same label
    for (size_t i = 0; i < coords.size(); ++i)
    {
        Node *current = graph.getNode(i);
        if (!current)
        {
            cout << "Node with id: " << i << " does not exist in the graph.\n";
            continue;
        }

        int currentLabel = current->getLabel();

        vector<int> targetNodes;

        if (currentLabel == -1)
        {
            // if the label is -1, then add nodes from all labels
            for (size_t j = 0; j < coords.size(); ++j)
            {
                if (j != i)
                {
                    targetNodes.push_back(j);
                }
            }
        }
        else
        {
            // else, find neighbors with the same label
            for (size_t j = 0; j < coords.size(); ++j)
            {
                if (j != i && graph.getNode(j)->getLabel() == currentLabel)
                {
                    targetNodes.push_back(j);
                }
            }
        }

        // add edges to up to 2 randomly selected target nodes
        unsigned seed = chrono::system_clock::now().time_since_epoch().count();
        shuffle(targetNodes.begin(), targetNodes.end(), default_random_engine(seed));

        int edgeCount = min(2, static_cast<int>(targetNodes.size()));
        for (int e = 0; e < edgeCount; ++e)
        {
            graph.addEdge(i, targetNodes[e]);
        }

        if (edgeCount == 0)
        {
            cerr << "Warning: Node " << i << " with label " << currentLabel
                 << " has no valid neighbors to connect." << endl;
        }
    }
}