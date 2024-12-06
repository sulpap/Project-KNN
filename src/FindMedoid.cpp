#include "../include/graph.hpp"

#include <cassert>
#include <algorithm>    // shuffle
#include <random>       // random_device
using namespace std;

map<int, Node*> FindMedoid(Graph &graph, int taph, set<int> F) {
    assert(taph >= 1);
    map<int, Node*> M;

    random_device rd;                     // True random seed generator
    mt19937 generator(rd());              // Mersenne Twister engine seeded with random device
    
    // Για κάθε φίλτρο f του συνόλου F
    for (auto f : F) {
        vector<Node*> P_f = graph.findNodesWithLabel(f);       // Βρίσκουμε τα nodes που έχουν το ίδιο φίλτρο
        if(P_f.size() == 0)
            continue;          // Δεν υπάρχουν στον γράφο, κόμβοι με φίλτρο = f (άρα, το M δεν θα περιέχει entry για το f)

        if (static_cast<int>(P_f.size()) < taph)      // Σύμφωνα με: https://piazza.com/class/m1kh0ggogpyg0/post/59
            taph = P_f.size();

        vector<Node*> shuffledNodes = P_f;              // Store a copy of P_f into shuffledNodes
        shuffle(shuffledNodes.begin(), shuffledNodes.end(), generator);         // Shuffle the vector randomly

        // Select the first taph elements
        vector<Node*> selectedNodes(shuffledNodes.begin(), shuffledNodes.begin() + taph);

        // We only need one element, δεδομένου ότι για εμάς ο μετρητής Τ δεν υπάρχει (see earlier mentioned piazza post)
        // Αφού στο selectedNodes τα Node* είναι ήδη shuffled, μάς αρκεί να πάρουμε το 1ο στοιχείου του selectedNodes
        M[f] = selectedNodes[0];
    }
    return M;
}

// Old FindMedoid with use of T:
// #include "../include/graph.hpp"
// #include <iostream>
// #include <fstream>
// #include <algorithm>
// #include <limits>
// #include <random>

// vector<int> getRandomSample(vector<int>& Pf, int taph) 
// {
//     // Edge case: if the number of requested samples is greater than the available data
//     if (static_cast<vector<int>::size_type>(taph) >= Pf.size()) {
//         return Pf; // Return all elements if we want more or equal samples than available
//     }

//     // Initialize random number generator --> this is faster than the default engine
//     random_device rd;
//     mt19937 gen(rd());

//     // Shuffle Pf to get random sampling
//     shuffle(Pf.begin(), Pf.end(), gen);

//     // Get the first 'taph' elements as the sampled result
//     vector<int> Rf(Pf.begin(), Pf.begin() + taph);

//     return Rf;
// }

// // p* <- arg min_{p in Rf} T[p]
// int findMinInT(const vector<int>& Rf, const map<int, int>& T) 
// {
//     int p_star = -1;
//     int min_value = numeric_limits<int>::max();

//     // Iterate through each point in Rf
//     for (const int& p : Rf) 
//     {
//         if (T.at(p) < min_value) { // Access T[p] safely using .at()
//             min_value = T.at(p);
//             p_star = p;
//         }
//     }

//     return p_star;
// }

// map<int, Node*> FindMedoid(Graph &graph, int taph, set<int> F)
// {
//     // 1. Initialize M to be an empty map and T to a zero map
//     map<int, Node *> M;
//     map<int, int> T; // T is intended as a counter

//     for (const auto& pair : graph.getAdjList()) 
//     {
//         T[pair.first] = 0; // initializing T to a zero map, ensures that it has entries for all nodes in the graph
//     }

//     // for each f ∈ F, the set of all filters, do
//     for(int f : F)
//     {
//         // 2. Let Pf denore the ids of all points matching filter f
//         vector<int> Pf = graph.findNodeIdsWithLabel(f);

//         if (Pf.empty())
//         {
//             cout << "Nodes not found with label: " << f << "!" << endl;
//             continue; // skip if no nodes have this label
//         }

//         // 3. Let Rf ← τ randomly sampled data point ids from Pf
//         vector<int> Rf = getRandomSample(Pf, taph); // taph is a threshold = an upper bound

//         if (Rf.empty())
//         {
//             cout << "Random samples not found for label: " << f << "!" << endl;
//             continue; // skip if no valid sample could be found
//         }

//         // p* <- arg min_{p ∈ Rf} T[p]
//         int p_star = findMinInT(Rf, T);

//         // Update M[f] <- p* and T[p*] <- T[p*] + 1
//         Node* p_star_node = graph.getNode(p_star);
//         if (p_star_node)
//         {
//             M[f] = p_star_node;
//             T[p_star] += 1; // or T[p_star]++;
//         }
//     }
//     return M;
// }