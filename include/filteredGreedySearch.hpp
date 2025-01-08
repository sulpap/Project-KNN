#ifndef FILTEREDGREEDYSEARCH_HPP
#define FILTEREDGREEDYSEARCH_HPP

#include "../include/graph.hpp"
#include <unordered_map>

// L_set, V_set should be empty when calling the function
// L_set, V_set will be full once function call terminated
// See first lines of implementation for better explanation of parameters
void FilteredGreedySearch(set<Node *> &S_set, vector<double> &queryCoords, int k, int L, set<Node *> &L_set, set<Node *> &V_set, set<int> &F_q_set);

// Hash function used for: unordered_map<pair<Node*, Node*>, double, PairHash> nodePairMap
struct PairHash {
    size_t operator()(const pair<Node*, Node*>& p) const {
        auto h1 = hash<Node*>()(p.first);
        auto h2 = hash<Node*>()(p.second);
        // Combine hashes in an order-independent way for symmetry
        return h1 ^ h2;             // xor
    }
};

// Δύο είναι οι διαφορές από τον από πάνω filtered greedy search: 1. Το query δίνεται σε Node* και όχι σε vector, 2. Λαμβάνει ως παράμετρο τις αποθηκευμένες αποστάσεις (nodePairMap)
// Γι' αυτούς του δύο λόγους, καλείται *μόνο* από τον filtered vamana, και *όχι* από την filtered main.
void FilteredGreedySearchIndex(set<Node *> &S_set, Node* query_node, int k, int L, set<Node *> &L_set, set<Node *> &V_set, set<int> &F_q_set, unordered_map<pair<Node*, Node*>, double, PairHash>& nodePairMap);

#endif