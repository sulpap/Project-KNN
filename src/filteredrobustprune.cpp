#include <set>
#include <limits>
#include <cmath>
#include "../include/graph.hpp"
#include "../include/utility.hpp"

void FilteredRobustPrune(Node* p, set<Node*> V, double a, int R) {
    // Step 1: V <- (V ∪ N_out(p)) \ {p}
    list<Node*> currentOutNeighbors = p->getEdges();
    V.insert(currentOutNeighbors.begin(), currentOutNeighbors.end());
    V.erase(p);

    // Step 2: N_out(p) <- ∅ (clear current out-neighbors of p)
    p->mutex_lock();
    p->setEdges(list<Node*>());
    p->mutex_unlock();

    // Step 3: while V ≠ ∅ do
    while (!V.empty()) {
        // Step 4: p* <- argmin_p'∈V (d(p, p')) (find closest neighbor)
        Node* pStar = nullptr;
        double minDistance = numeric_limits<double>::max();

        for (Node* pPrime : V) {
            double dist = euclidean_distance_of_nodes(p, pPrime);
            if (dist < minDistance) {
                minDistance = dist;
                pStar = pPrime;
            }
        }

        // Step 5: N_out(p) <- N_out(p) ∪ {p*}
        p->mutex_lock();
        p->addEdge(pStar);
        p->mutex_unlock();

        // Step 6: if |N_out(p)| = R then break
        if (p->getEdges().size() == static_cast<size_t>(R)) {
            break;
        }

        // Step 7: for p'∈V do
        auto it = V.begin();
        while (it != V.end()) {
            Node* pPrime = *it;
            // Step 8: Check label compatibility
            // if (pPrime->getLabel() != -1 && p->getLabel() != -1) {
                // if ((p->getLabel() == pPrime->getLabel()) && (p->getLabel() != pStar->getLabel())) {
                //     ++it;
                //     cout << "inside continue if" << endl;
                //     continue;
                // }
            // }
            // if ((p->getLabel() == pPrime->getLabel()) && (p->getLabel() != pStar->getLabel())) {
            //     ++it;
            //     continue;
            // }

            if (pPrime->getLabel() != pStar->getLabel()) {
                ++it;
                continue;
            }

            // Step 9: Check distance-based filtering
            if (a * euclidean_distance_of_nodes(pStar, pPrime) <= euclidean_distance_of_nodes(p, pPrime)) {
                // Remove p' from V
                it = V.erase(it);
            } else {
                ++it;
            }
        }
    }
}


// serial:
// void FilteredRobustPrune(Node* p, set<Node*> V, double a, int R) {
//     // Step 1: V <- (V ∪ N_out(p)) \ {p}
//     list<Node*> currentOutNeighbors = p->getEdges();
//     V.insert(currentOutNeighbors.begin(), currentOutNeighbors.end());
//     V.erase(p);

//     // Step 2: N_out(p) <- ∅ (clear current out-neighbors of p)
//     p->setEdges(list<Node*>());

//     // Step 3: while V ≠ ∅ do
//     while (!V.empty()) {
//         // Step 4: p* <- argmin_p'∈V (d(p, p')) (find closest neighbor)
//         Node* pStar = nullptr;
//         double minDistance = numeric_limits<double>::max();

//         for (Node* pPrime : V) {
//             double dist = euclidean_distance_of_nodes(p, pPrime);
//             if (dist < minDistance) {
//                 minDistance = dist;
//                 pStar = pPrime;
//             }
//         }

//         // Step 5: N_out(p) <- N_out(p) ∪ {p*}
//         p->addEdge(pStar);

//         // Step 6: if |N_out(p)| = R then break
//         if (p->getEdges().size() == static_cast<size_t>(R)) {
//             break;
//         }

//         // Step 7: for p'∈V do
//         auto it = V.begin();
//         while (it != V.end()) {
//             Node* pPrime = *it;
//             // Step 8: Check label compatibility
//             // if (pPrime->getLabel() != -1 && p->getLabel() != -1) {
//                 // if ((p->getLabel() == pPrime->getLabel()) && (p->getLabel() != pStar->getLabel())) {
//                 //     ++it;
//                 //     cout << "inside continue if" << endl;
//                 //     continue;
//                 // }
//             // }
//             // if ((p->getLabel() == pPrime->getLabel()) && (p->getLabel() != pStar->getLabel())) {
//             //     ++it;
//             //     continue;
//             // }

//             if (pPrime->getLabel() != pStar->getLabel()) {
//                 ++it;
//                 continue;
//             }

//             // Step 9: Check distance-based filtering
//             if (a * euclidean_distance_of_nodes(pStar, pPrime) <= euclidean_distance_of_nodes(p, pPrime)) {
//                 // Remove p' from V
//                 it = V.erase(it);
//             } else {
//                 ++it;
//             }
//         }
//     }
// }
