#include <vector>
#include "../include/graph.hpp"
#include <unordered_map>

using namespace std;

// L_set, V_set should be empty when calling the function
// L_set, V_set will be full once function call terminated
void GreedySearch(Node *start_node, vector<double> &queryCoords, int k, int L, set<Node *> &L_set, set<Node *> &V_set);

// Hash function used for: unordered_map<pair<Node*, Node*>, double, PairHash> nodePairMap
struct PairHash {
    size_t operator()(const pair<Node*, Node*>& p) const {
        auto h1 = hash<Node*>()(p.first);
        auto h2 = hash<Node*>()(p.second);
        // Combine hashes in an order-independent way for symmetry
        return h1 ^ h2;             // xor
    }
};

// Δύο είναι οι διαφορές από τον από πάνω greedy search: 1. Το query δίνεται σε Node* και όχι σε vector, 2. Λαμβάνει ως παράμετρο τις αποθηκευμένες αποστάσεις (nodePairMap)
// IMPORTANT: Γι' αυτούς του δύο λόγους, καλείται *μόνο* από τον vamana, και *όχι* από την first_main.
void GreedySearchIndex(Node* start_node, Node* query_node, int k, int L, set<Node*> &L_set, set<Node*> &V_set, unordered_map<pair<Node*, Node*>, double, PairHash>& nodePairMap);