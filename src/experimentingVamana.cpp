#include "../include/graph.hpp"
#include "../include/robustprune.hpp"
#include "../include/greedysearch.hpp"
#include <algorithm>
#include <random>
#include <cmath>

Graph BuildDirectedGraph(Graph& P, int L, int R, double a) {
    Graph G;

    // Step 1: Initialize G to a random R-regular directed graph over the nodes in P
    for (auto& [id, node] : P.getAdjList()) {
        G.addNode(new Node(*node));
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    for (auto& [id, node] : G.getAdjList()) {
        std::vector<int> neighbors;
        for (auto& [neighbor_id, _] : G.getAdjList()) {
            if (neighbor_id != id) {
                neighbors.push_back(neighbor_id);
            }
        }
        std::shuffle(neighbors.begin(), neighbors.end(), gen);
        for (int i = 0; i < R; ++i) {
            G.addEdge(id, neighbors[i]);
        }
    }

    // Step 2: Determine the medoid of the dataset P
    Node* medoid = nullptr;
    double min_sum_distance = std::numeric_limits<double>::max();
    for (auto& [id, node] : P.getAdjList()) {
        double sum_distance = 0.0;
        for (auto& [_, other_node] : P.getAdjList()) {
            for (size_t d = 0; d < node->getCoordinates().size(); ++d) {
                sum_distance += std::pow(node->getCoordinates()[d] - other_node->getCoordinates()[d], 2);
            }
        }
        if (sum_distance < min_sum_distance) {
            min_sum_distance = sum_distance;
            medoid = node;
        }
    }

    // Step 3: Random permutation σ of 1..n
    std::vector<int> permutation(P.getAdjList().size());
    std::iota(permutation.begin(), permutation.end(), 1);
    std::shuffle(permutation.begin(), permutation.end(), gen);

    // Step 4: Process each node in the order defined by σ
    for (int idx : permutation) {
        Node* node = P.getNode(idx);
        std::set<int> L_set, V_set;

        // GreedySearch to get L_set and V_set
        // GreedySearch(P, medoid->getId(), node->getCoordinates(), 1, L, L_set, V_set);

        // Transform V_set into a set of Node* for RobustPrune
        std::set<Node*> V;
        for (int id : V_set) {
            V.insert(P.getNode(id));
        }

        // RobustPrune to update the out-neighbors of σ(i)
        RobustPrune(G, node, V, a, R);

        // Step 5: Adjust out-neighbors to respect degree bound
        for (Node* neighbor : node->getEdges()) {
            if (neighbor->getEdges().size() + 1 > static_cast<size_t>(R)) {
                std::set<Node*> neighbor_out_neighbors;
                for (Node* n : neighbor->getEdges()) {
                    neighbor_out_neighbors.insert(n);
                }
                neighbor_out_neighbors.insert(node);
                RobustPrune(G, neighbor, neighbor_out_neighbors, a, R);
            } else {
                G.addEdge(neighbor->getId(), node);
            }
        }
    }

    return G;
}
