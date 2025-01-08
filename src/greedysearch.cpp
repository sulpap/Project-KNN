#include "../include/greedysearch.hpp"
#include "../include/utility.hpp"

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

void GreedySearch(Node* start_node, vector<double> &queryCoords, int k, int L, set<Node*> &L_set, set<Node*> &V_set) {

    // All the elements in a set have unique values
    // by default set is sorted in ascending order
    assert(L_set.empty() == true);      // given L_set should be empty
    assert(V_set.empty() == true);      // given V_set should be empty
    assert(L >= k);
    assert(start_node != NULL);
    assert(queryCoords.size() > 0);

    unordered_map<Node*, double> nodeMap;           // Main storage
    set<pair<double, Node*>, Compare> sortedSet;    // Secondary sorted viewm of main storage
    set<pair<double, Node*>, Compare> LminusV;

    double dist = euclidean_distance(start_node->getCoordinates(), queryCoords);      
    insert(nodeMap, sortedSet, start_node, dist, L);                            // Initialization of L_set
    // V_set is empty

    // L\V = {start} \ {} = {start}
    LminusV.emplace(dist, start_node);                            // Inserts a new pair in the set, if unique. This new pair is constructed in place using args as the arguments for its construction.

    while(LminusV.empty() == false) {       // while LminusV != {}        

        auto first_pair = *LminusV.begin();                     // LminusV is sorted based on distance in descending order. Therefore, first element of LminusV is the min we are looking for
        Node* p_star = first_pair.second;                       // The 'Node*'        

        // Update V_set [V = V U p*]
        V_set.insert(p_star);                                   // if p_star is already in V_set, p_set won't be inserted in V

        
        // Update L_set [ L = L U Nout(p*) ]
        list<Node*> p_star_out = p_star->getEdges();            // out-neighbors of p*
        for(auto node : p_star_out) {                           // Insert all of out-neighbors of p* into L set
            dist = euclidean_distance(node->getCoordinates(), queryCoords);
            insert(nodeMap, sortedSet, node, dist, L);
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