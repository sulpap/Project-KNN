/* Contains helper functions in populating the graphs */

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
    {
        cerr << "Error [generate_graph]: No coordinates provided." << endl;
        return; // avoid processing empty graphs
    }

    // nodeIds start from 0
    for (size_t i = 0; i < coords.size(); i++)
    {
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

        // randomly add edges until the node reaches adjusted_R edges or no more neighbors are available
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

#include <omp.h>

void generate_graph_parallel(Graph &graph, vector<Node *> &coords, int R)
{
    srand(time(0)); 

    size_t n = coords.size(); // number of points in the current graph
    if (n == 0)
    {
        cerr << "Error [generate_graph]: No coordinates provided." << endl;
        return; // avoid processing empty graphs
    }

    // adjust R if necessary
    int adjusted_R = min(static_cast<int>(n) - 1, R);

    // Step 1: Parallel addition of nodes
    #pragma omp parallel for schedule(static)
    for (size_t i = 0; i < coords.size(); ++i)
    {
        #pragma omp critical
        {
            graph.addNode(coords[i]);
        }
    }

    // Step 2: Parallel edge creation
    #pragma omp parallel for schedule(static)
    for (size_t i = 0; i < coords.size(); ++i)
    {
        Node *current = graph.getNode(coords[i]->getId());
        if (!current)
        {
            #pragma omp critical
            {
                cout << "Node with id: " << coords[i]->getId() << " does not exist in the graph.\n";
            }
            continue;
        }

        int edgesAdded = 0;
        set<size_t> potentialNeighbors;

        // add other nodes as potential neighbors
        for (size_t j = 0; j < n; ++j)
        {
            if (j != i)
            {
                potentialNeighbors.insert(coords[j]->getId());
            }
        }

        // randomly add edges until the node reaches adjusted_R edges or no more neighbors are available
        while (edgesAdded < adjusted_R && !potentialNeighbors.empty())
        {
            auto it = potentialNeighbors.begin();
            advance(it, rand() % potentialNeighbors.size()); // select a random neighbor

            size_t randomNeighborId = *it;

            #pragma omp critical
            {
                if (!current->edgeExists(randomNeighborId))
                {
                    graph.addEdge(coords[i]->getId(), randomNeighborId);
                    edgesAdded++;
                }
            }

            potentialNeighbors.erase(it); // remove the selected neighbor to avoid cycles!
        }

        if (edgesAdded < adjusted_R)
        {
            #pragma omp critical
            {
                printf("Warning: Node %ld could only form %d edges.\n", i, edgesAdded);
            }
        }
    }
}

// function to randomly connect nodes to nodes with the same label
void generate_label_based_graph(Graph &graph, const vector<vector<double>> &coords)
{
    srand(time(0));

    size_t n = coords.size(); // number of points in the current graph
    if (n == 0)
    {
        cerr << "Error [generate_label_based_graph]: No coordinates provided." << endl;
        return; // avoid processing empty graphs
    }

    map<int, vector<int>> labelToNodes; // this is to group nodes by label

    // group nodes by label (nodes are already added by the initialization function)
    for (size_t i = 0; i < n; ++i)
    {
        int label = static_cast<int>(coords[i][0]); // The label is the 0th element of the coords
        labelToNodes[label].push_back(i);
    }

    // add edges --> max 1 edge to a node of the same label
    for (size_t i = 0; i < n; ++i)
    {
        Node *current = graph.getNode(i);
        if (!current)
        {
            // cerr << "Error: Node with id " << i << " does not exist in the graph." << endl;
            continue;
        }

        int label = current->getLabel();
        const auto &sameLabelNodes = labelToNodes[label];

        // if no other nodes with the same label exist, skip
        if (sameLabelNodes.size() <= 1)
        {
            // cerr << "Warning: Node " << i << " has no other nodes with the same label to connect to." << endl;
            continue;
        }

        // select a random neighbor with the same label
        vector<int> potentialNeighbors;
        for (int neighborId : sameLabelNodes)
        {
            if (neighborId != static_cast<int>(i))
            {
                potentialNeighbors.push_back(neighborId);
            }
        }

        if (!potentialNeighbors.empty())
        {
            unsigned seed = chrono::system_clock::now().time_since_epoch().count();
            shuffle(potentialNeighbors.begin(), potentialNeighbors.end(), default_random_engine(seed));
            int randomNeighborId = potentialNeighbors.front();

            if (!current->edgeExists(randomNeighborId))
            {
                graph.addEdge(i, randomNeighborId);
            }
        }
    }
}

// generates a specified number of random edges for each node in the graph
void generate_random_edges(Graph &graph, int maxEdgesPerNode)
{
    srand(time(0));

    auto adjList = graph.getAdjList(); // get the adjacency list from the graph
    size_t n = adjList.size();         // number of nodes in the graph
    // size_t n = coords.size(); // number of points in the current graph

    if (n == 0)
    {
        cerr << "Error [generate_random_edges]: No coordinates provided." << endl;
        return; // avoid processing empty graphs
    }

    // add edges --> up to maxEdgesPerNode
    for (const auto &entry : adjList)
    {
        Node *current = entry.second;
        if (!current)
        {
            continue;
        }

        vector<int> potentialNeighbors; // to store potential neighbors

        // collect potential neighbors (all other nodes that are not already connected)
        for (const auto &otherEntry : adjList)
        {
            int neighborId = otherEntry.first;
            if (neighborId != current->getId() && !current->edgeExists(neighborId))
            {
                potentialNeighbors.push_back(neighborId);
            }
        }

        // if there are no potential neighbors, skip
        if (potentialNeighbors.empty())
        {
            continue;
        }

        // shuffle potential neighbors to randomize
        random_device rd;
        mt19937 gen(rd()); // Mersenne Twister provides better randomization than the default engine
        shuffle(potentialNeighbors.begin(), potentialNeighbors.end(), gen);

        // add up to maxEdgesPerNode edges
        for (int k = 0; k < maxEdgesPerNode && k < static_cast<int>(potentialNeighbors.size()); ++k)
        {
            graph.addEdge(current->getId(), potentialNeighbors[k]);
        }
    }
}

void connect_subgraphs(Graph &globalGraph, unordered_map<int, vector<Node *>> &PfMap)
{
    srand(time(0));

    vector<int> labels;
    for (const auto &entry : PfMap)
    {
        labels.push_back(entry.first);
    }

    if (labels.size() < 2)
    {
        cerr << "Error: Not enough subgraphs to connect." << endl;
        return;
    }

    // shuffle labels to randomize connection order
    random_device rd;
    mt19937 gen(rd());
    shuffle(labels.begin(), labels.end(), gen);

    // do random connections making sure each subgraph is connected to at least one other subgraph
    for (size_t i = 0; i < labels.size() - 1; ++i)
    {
        int label1 = labels[i];
        int label2 = labels[i + 1];

        const auto &nodes1 = PfMap[label1];
        const auto &nodes2 = PfMap[label2];

        if (nodes1.empty() || nodes2.empty())
        {
            continue; // skip if no nodes in either subgraph
        }

        // pick random nodes from each subgraph
        Node *node1 = nodes1[rand() % nodes1.size()];
        Node *node2 = nodes2[rand() % nodes2.size()];

        if (!node1->edgeExists(node2->getId()))
        {
            globalGraph.addEdge(node1->getId(), node2->getId());
            // cout << "Connecting node " << node1->getId() << " (label " << label1 << ") to node " << node2->getId() << " (label " << label2 << ")\n";
        }
    }
}