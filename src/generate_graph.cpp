#include <../include/utility.hpp>
#include "../include/graph.hpp"
#include <cstdlib>
#include <ctime>

void generate_graph(Graph &graph, vector<vector<double>> &coords, int maxNodeEdges)
{
    srand(time(0));

    // nodeIds start from 1
    for (size_t i = 1; i <= coords.size(); i++) 
    {
        Node* newNode = new Node(i, coords[i - 1], {}); // vector coords start from 0
        graph.addNode(newNode);
    }

    //add random edges between the nodes
    for (size_t i = 1; i <= coords.size(); ++i) 
    {
        Node* current = graph.getNode(i);

        // random number of edges. at least 1 per node.
        int edgesToAdd = rand() % maxNodeEdges + 1;

        for (int j = 0; j < edgesToAdd; ++j) 
        {
            // random destination node
            size_t randomNeighborId = rand() % coords.size() + 1;

            //we don't add an edge to the same node or if the edge already exists
            if (randomNeighborId != i && !current->edgeExists(randomNeighborId)) {
                graph.addEdge(i, randomNeighborId);
            }
        }
    }

}