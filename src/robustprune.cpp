#include <set>
#include <limits>
#include <cmath>
#include "../include/graph.hpp"
#include "../include/utility.hpp"

void RobustPrune(Node* p, set<Node*> V, double a, int R) {
    // Step 1: V <- (V ∪ N_out(p)) \ {p}
    list<Node*> currentOutNeighbors = p->getEdges();
    V.insert(currentOutNeighbors.begin(), currentOutNeighbors.end());
    V.erase(p);

    // Step 2: N_out(p) <- ∅ (clear current out-neighbors of p)
    p->setEdges(list<Node*>());

    // Step 3: while V ≠ ∅ do
    while (!V.empty()) {
        // Step 4: p* <- argmin_p'∈V (d(p, p')) (find closest neighbor)
        Node* pStar = nullptr;
        double minDistance = std::numeric_limits<double>::max();

        for (Node* pPrime : V) {
            double dist = euclidean_distance_of_nodes(p, pPrime);
            if (dist < minDistance) {
                minDistance = dist;
                pStar = pPrime;
            }
        }

        // Step 5: N_out(p) <- N_out(p) ∪ {p*}
        p->addEdge(pStar);

        // Step 6: if |N_out(p)| = R then break
        if (p->getEdges().size() == static_cast<size_t>(R)) {
            break;
        }

        // Step 7: for p'∈V do
        auto it = V.begin();
        while (it != V.end()) {
            Node* pPrime = *it;
            if (a * euclidean_distance_of_nodes(pStar, pPrime) <= euclidean_distance_of_nodes(p, pPrime)) {
                // Step 8: remove p' from V
                it = V.erase(it);
            } else {
                ++it;
            }
        }
    }
}
