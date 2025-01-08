#include "../include/filteredGreedySearch.hpp"
#include "../include/utility.hpp"               // due to eucledean_distance

#include <cassert>
#include <unordered_map>

// Compare function for sets that store: pair<double, Node*>. We want the set to be sorted by dist, and when tie breaker to use node's address
struct Compare {
    bool operator()(const pair<double, Node*>& a, const pair<double, Node*>& b) const {
        if (a.first != b.first)
            return a.first < b.first;   // Sort by dist
        return a.second < b.second;     // Tie-breaker: Node* address
    }
};

// Insert function with size limit enforcement (our own insert function for inserting in "L set" -- updates nodeMap, sortedSet and checks if up to L elements are stored)
static void insert(unordered_map<Node*, double>& nodeMap,
            set<pair<double, Node*>, Compare>& sortedSet,
            Node* node, double dist, size_t maxSize) {
    if (nodeMap.find(node) != nodeMap.end()) {
        // Node already exists. Skip this node (a set should contain unique elements)
        return;
    }

    // Insert into both containers
    nodeMap[node] = dist;
    sortedSet.emplace(dist, node);

    // Enforce size limit
    if (sortedSet.size() > maxSize) {       // due to L should have up to L elements (see pseudocode)
        // Remove the largest element
        auto it = prev(sortedSet.end());
        Node* nodeToRemove = it->second;    // Get Node* from the set
        nodeMap.erase(nodeToRemove);        // Remove from map
        sortedSet.erase(it);                // Remove from set
    }
}

// Our own set_difference function for finding the difference of two sets of the following form: set<pair<double, Node*> vs set<Node*>& V_set
static set<pair<double, Node*>, Compare> set_difference(
    const set<pair<double, Node*>, Compare>& sortedSet,
    const set<Node*>& V_set) {
   
    set<pair<double, Node*>, Compare> result;

    for (const auto& pair : sortedSet) {
        // Check if the Node* (second element of the pair) is in V_set
        if (V_set.find(pair.second) == V_set.end()) {
            // Not found in V_set, include in the result
            result.insert(pair);
        }
    }

    return result;
}


// L_set, V_set should be empty when calling the function
// L_set, V_set will be full once function call terminated
void FilteredGreedySearch(set<Node*> &S_set, vector<double> &queryCoords, int k, int L, set<Node*> &L_set, set<Node*> &V_set, set<int> &F_q_set){ 
    /*
    set<Node*> &S_set;
    Όταν ο fgs (filtered greedy seach) καλείται από τον vamana, τότε το S_set περιέχει μόνο έναν κόμβο.
    Όταν o fgs καλείται από τη main για query με φίλτρο, τότε το S_set περιέχει μόνο έναν κόμβο.
    Όταν o fgs καλείται από τη main για query χωρίς φίλτρο, τότε το S_set περιέχει έναν starting κόμβο για κάθε φίλτρο.
    Από τα παραπάνω καταλαβαίνουμε ότι το S χρειάζεται να είναι ένα set, καθώς υπάρχει περίπτωση να είναι > 1 στοιχείο.
    
    vector<double> &queryCoords;
    Όταν ο fgs καλείται από τον vamana, τότε το queryCoords περιέχει τις συντεταγμένες του εκάστοτε σημείου που εξετάζουμε 
    (μόνο τις συντεταγμένες! όχι το φίλτρο)
    Όταν ο fgs καλείται από τη main, τότε το queryCoords περιέχει τις συντεταγμένες του query 
    (μόνο τις συντεταγμένες! όχι το φίλτρο)

    int k;
    Όταν ο fgs καλείται από τον vamana, τότε k = 0
    Όταν ο fgs καλείται από τη main, τότε k = 100

    int L;

    set<Node*> &L_set;
    Όταν ο fgs καλείται από τον vamana, τότε το L_set που επιστρέφουμε θα είναι κενό.
    Όταν ο fgs καλείται από τη main, τότε από το L_set θα χρειαστούμε μόνο τα node ids. Οπότε μπορεί να οριστεί και ως: set<int> &L_set

    set<Node*> &V_set;

    set<int> &F_q_set;
    Όταν ο fgs καλείται από τον vamana, τότε το F_q_set περιέχει το φίλτρο του εκάστοτε σημείου που εξετάζουμε. 
    Όταν o fgs καλείται από τη main για query με φίλτρο, το F_q_set περιέχει το φίλτρο του query.
    Όταν o fgs καλείται από τη main για query χωρίς φίλτρο, τότε το F_q_set περιέχει κάθε φίλτρο.
    */


    assert(L >= k);
    assert(queryCoords.size() > 0);
    // Initialize L_set, V_set to empty
    assert(L_set.empty() == true);      // given L_set should be empty
    assert(V_set.empty() == true);      // given V_set should be empty
    if(S_set.size() == 0)               // Δεν υπάρχει start node για το συγκεκριμένο φίλτρο (aka, δεν υπάρχει point στον γράφο με αυτό το φίλτρο)
        return;                         // therefore, we return empty L_set, V_set


    unordered_map<Node*, double> nodeMap;           // Main storage
    set<pair<double, Node*>, Compare> sortedSet;    // Secondary sorted viewm of main storage
    set<pair<double, Node*>, Compare> LminusV;

    // We traverse S_set:
    set<int> s_filters;
    for (auto node : S_set) {
        s_filters.insert(node->getLabel());
        double dist = euclidean_distance(node->getCoordinates(), queryCoords);
        insert(nodeMap, sortedSet, node, dist, L);              // insert into "L set"
        LminusV.emplace(dist, node);                            // Inserts a new pair in the set, if unique. This new pair is constructed in place using args as the arguments for its construction.
    }   
    assert(s_filters == F_q_set);                               // same size & same content

    while(LminusV.empty() == false) {                           // while LminusV != {}        

        auto first_pair = *LminusV.begin();                     // LminusV is sorted based on distance in descending order. Therefore, first element of LminusV is the min we are looking for
        Node* p_star = first_pair.second;                       // The 'Node*'        

        // Update V_set [V = V U p*]
        V_set.insert(p_star);                                   // if p_star is already in V_set, p_set won't be inserted in V


        // Update L_set [ L = L U Nout(p*) ]
        list<Node*> p_star_out = p_star->getEdges();            // out-neighbors of p*
        for(auto node : p_star_out) {
            int filter_p_star_out = node->getLabel();
            
            if(F_q_set.find(filter_p_star_out) == F_q_set.end()) {
                continue;                                       // we don't take this neighbor into account, since its filter doesn't exist in F_q_set
            }

            if(V_set.find(node) == V_set.end()) {               // p_star_out doesn't exist in V
                double dist = euclidean_distance(node->getCoordinates(), queryCoords);
                insert(nodeMap, sortedSet, node, dist, L);      // insert into "L set"
            }
        }

        // Update of L to retain top L elements of vector happens immediately whenever we insert an element in "L set"

        LminusV = set_difference(sortedSet, V_set);
    }

    // We update L to retain top k elements of vector
    int my_k = k;
    if(static_cast<int>(sortedSet.size()) < k) {
        my_k = sortedSet.size();
    }

    int i = 0;
    for (auto it = sortedSet.begin(); i < my_k; ++it, ++i) {
        L_set.insert(it->second);  // `it->second` is the node
    }

    return;
}

// IMPORTANT: Should be called *only* from filtered vamana, *not* from filtered main.
void FilteredGreedySearchIndex(set<Node *> &S_set, Node* query_node, int k, int L, set<Node *> &L_set, set<Node *> &V_set, set<int> &F_q_set, unordered_map<pair<Node*, Node*>, double, PairHash>& nodePairMap) {
    assert(L >= k);
    assert(query_node->getCoordinates().size() > 0);
    // Initialize L_set, V_set to empty
    assert(L_set.empty() == true);      // given L_set should be empty
    assert(V_set.empty() == true);      // given V_set should be empty
    if(S_set.size() == 0)               // Δεν υπάρχει start node για το συγκεκριμένο φίλτρο (aka, δεν υπάρχει point στον γράφο με αυτό το φίλτρο)
        return;                         // therefore, we return empty L_set, V_set


    unordered_map<Node*, double> nodeMap;           // Main storage
    set<pair<double, Node*>, Compare> sortedSet;    // Secondary sorted viewm of main storage
    set<pair<double, Node*>, Compare> LminusV;

    // We traverse S_set:
    set<int> s_filters;
    for (auto node : S_set) {
        s_filters.insert(node->getLabel());

        // Create an order-independent key
        pair<Node*, Node*> key = {min(node, query_node), max(node, query_node)};        // The hash and key use min and max to ensure that {node1, node2} is treated the same as {node2, node1}. This eliminates the need to insert the symmetric pair manually.
        assert(nodePairMap.find(key) != nodePairMap.end());     // assert if key doesn't exist
        double dist = nodePairMap[key];                         // already precomputed!
        insert(nodeMap, sortedSet, node, dist, L);              // insert into "L set"
        LminusV.emplace(dist, node);                            // Inserts a new pair in the set, if unique. This new pair is constructed in place using args as the arguments for its construction.
    
    }   
    assert(s_filters == F_q_set);                               // same size & same content

    while(LminusV.empty() == false) {                           // while LminusV != {}        

        auto first_pair = *LminusV.begin();                     // LminusV is sorted based on distance in descending order. Therefore, first element of LminusV is the min we are looking for
        Node* p_star = first_pair.second;                       // The 'Node*'        

        // Update V_set [V = V U p*]
        V_set.insert(p_star);                                   // if p_star is already in V_set, p_set won't be inserted in V


        // Update L_set [ L = L U Nout(p*) ]
        list<Node*> p_star_out = p_star->getEdges();            // out-neighbors of p*
        for(auto node : p_star_out) {
            int filter_p_star_out = node->getLabel();
            
            if(F_q_set.find(filter_p_star_out) == F_q_set.end()) {
                continue;                                       // we don't take this neighbor into account, since its filter doesn't exist in F_q_set
            }

            if(V_set.find(node) == V_set.end()) {               // p_star_out doesn't exist in V
                pair<Node*, Node*> key = {min(node, query_node), max(node, query_node)};        // Create an order-independent key
                assert(nodePairMap.find(key) != nodePairMap.end());                             // assert if key doesn't exist
                double dist = nodePairMap[key];                 // already precomputed!
                insert(nodeMap, sortedSet, node, dist, L);      // insert into "L set"
            }
        }

        // Update of L to retain top L elements of vector happens immediately whenever we insert an element in "L set"

        LminusV = set_difference(sortedSet, V_set);
    }

    // We update L to retain top k elements of vector
    int my_k = k;
    if(static_cast<int>(sortedSet.size()) < k) {
        my_k = sortedSet.size();
    }

    int i = 0;
    for (auto it = sortedSet.begin(); i < my_k; ++it, ++i) {
        L_set.insert(it->second);  // `it->second` is the node
    }

    return;
}