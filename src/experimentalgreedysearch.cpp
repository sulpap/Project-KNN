#include <set>
#include <queue>
#include <cmath>
#include <algorithm>
#include "../include/graph.hpp"
#include "../include/utility.hpp"
#include "../include/experimentalgreedysearh.hpp"

// GreedySearch function
void GreedySearch(Graph& G, Node* s, vector<double>& x_q, int k, int L, set<Node*> &back_L_set, set<Node*> &V_set) {
    // Initialize the result set L_set and the visited set V
    set<Node*> V;
    vector<Node*> L_set = { s };
    V.insert(s);

    while (!L_set.empty()) {
        // Step 1: Find the closest node p* in L_set \ V to x_q
        Node* pStar = nullptr;
        double minDistance = numeric_limits<double>::max();
        for (Node* node : L_set) {
            if (V.find(node) == V.end()) { // Only consider nodes not in V
                double dist = euclidean_distance(node->getCoordinates(), x_q);
                if (dist < minDistance) {
                    minDistance = dist;
                    pStar = node;
                }
            }
        }

        // Step 2: Stop if no unvisited node remains
        if (pStar == nullptr) break;

        // Step 3: Update L_set and V
        for (Node* neighbor : pStar->getEdges()) {
            if (find(L_set.begin(), L_set.end(), neighbor) == L_set.end()) {
                L_set.push_back(neighbor);
            }
        }
        V.insert(pStar);

        // Step 4: Prune L_set if it exceeds L by retaining the closest L points to x_q
        if (L_set.size() > static_cast<size_t>(L)) {
            sort(L_set.begin(), L_set.end(), [&](Node* a, Node* b) {
                return euclidean_distance(a->getCoordinates(), x_q) < euclidean_distance(b->getCoordinates(), x_q);
            });
            L_set.resize(L);
        }
    }

    // Sort and return the closest k points from L_set and the visited nodes V
    sort(L_set.begin(), L_set.end(), [&](Node* a, Node* b) {
        return euclidean_distance(a->getCoordinates(), x_q) < euclidean_distance(b->getCoordinates(), x_q);
    });
    if (L_set.size() > static_cast<size_t>(k)) {
        L_set.resize(k);
    }

    for (Node* l : L_set) {
        back_L_set.insert(l);
    }
}