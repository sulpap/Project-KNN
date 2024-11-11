#include <../include/utility.hpp>
#include "../include/graph.hpp"
#include <cstdlib>
#include <ctime>

void generate_graph(Graph &graph, vector<vector<double>> &coords, int R)
{
    srand(time(0));

    // nodeIds start from 0
    for (size_t i = 0; i < coords.size(); i++) 
    {
        Node* newNode = new Node(i, coords[i], {}, {});
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
