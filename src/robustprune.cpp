#include <set>
#include <limits>
#include <cmath>
#include "../include/graph.hpp"

void RobustPrune(Node* p, set<Node*> V, double a, int R, Graph& G) {
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
            double dist = distance(p, pPrime); // Assuming you have a distance function
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
            if (a * distance(pStar, pPrime) <= distance(p, pPrime)) {
                // Step 8: remove p' from V
                it = V.erase(it);
            } else {
                ++it;
            }
        }
    }
}

// Distance function assuming Euclidean distance
double distance(Node* p1, Node* p2) {
    const vector<double>& coord1 = p1->getCoordinates();
    const vector<double>& coord2 = p2->getCoordinates();
    
    if (coord1.size() != coord2.size()) {
        throw runtime_error("Dimension mismatch in distance calculation.");
    }

    double sum = 0.0;
    for (size_t i = 0; i < coord1.size(); ++i) {
        sum += pow(coord1[i] - coord2[i], 2);
    }

    return sqrt(sum);
}