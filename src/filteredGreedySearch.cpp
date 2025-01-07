#include "../include/filteredGreedySearch.hpp"
#include "../include/utility.hpp"               // due to eucledean_distance

#include <cassert>
#include <cfloat>       // due to DBL_MAX
#include <algorithm>    // due to sort

struct info {
    Node* node;
    double distance;
};

static bool info_compare(const struct info &item_1, const struct info &item_2) {
    return item_1.distance < item_2.distance;
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
    if(S_set.size() == 0)       // Δεν υπάρχει start node για το συγκεκριμένο φίλτρο (aka, δεν υπάρχει point στον γράφο με αυτό το φίλτρο)
        return;                 // therefore, we return empty L_set, V_set

    // We traverse S_set:
    set<int> s_filters;
    set<Node*> LminusV;
    for (auto node : S_set) {
        L_set.insert(node);
        s_filters.insert(node->getLabel());
        LminusV.insert(node);
    }   
    assert(s_filters == F_q_set);           // same size & same content

    while(LminusV.empty() == false) {       // while LminusV != {}        

        double min_dist = DBL_MAX;
        Node* min_node = NULL;

        // We traverse LminusV:
        for (auto node : LminusV) {
            vector<double> node_coord = node->getCoordinates();
            double distance = euclidean_distance(node_coord, queryCoords);
            if(distance < min_dist) {
                min_dist = distance;
                min_node = node;
            }
        }  
        Node* p_star = min_node;

       // Update V_set [V = V U p*]
        V_set.insert(p_star);       // if p_star is already in V_set, p_set won't be inserted in V
      
        // Update L_set [ L = L U Nout(p*) ]
        list<Node*> p_star_out = p_star->getEdges();            // out-neighbors of p*
        for(auto node : p_star_out) {
            int filter_p_star_out = node->getLabel();

            if(F_q_set.find(filter_p_star_out) == F_q_set.end()) {
                continue;               // we don't take this neighbor into account, since its filter doesn't exist in F_q_set
            }

            if(V_set.find(node) == V_set.end()) {       // p_star_out doesn't exist in V
                L_set.insert(node);
            }
        }

        // Update L to retain closests L points to query
        if(static_cast<int>(L_set.size()) > L) {                
            vector<struct info> vector_info;
            // We traverse L and save it to vector
            for (auto node : L_set) {
                struct info item;
                item.node = node;
                item.distance = euclidean_distance(node->getCoordinates(), queryCoords);
                vector_info.push_back(item);
            }

            // We sort the vector in ascending order
            sort(vector_info.begin(), vector_info.end(), info_compare);
            
            // We update L to retain top L elements of vector
            set<Node*> temp;
            for(int i = 0; i < L; i++) {
                struct info item = vector_info[i];
                temp.insert(item.node);
            }
            L_set = temp;
        }

        set<Node*> temp;
        set_difference(L_set.begin(), L_set.end(), V_set.begin(), V_set.end(), inserter(temp, temp.begin()));
        LminusV = temp;
    }


    // Update L to retain closests k points to query
    vector<struct info> vector_info;
    // We traverse L and save it to vector
    for (auto node : L_set) {
        struct info item;
        item.node = node;
        item.distance = euclidean_distance(node->getCoordinates(), queryCoords);
        vector_info.push_back(item);
    }

    // We sort the vector in ascending order
    sort(vector_info.begin(), vector_info.end(), info_compare);
   
    // We update L to retain top k elements of vector
    int my_k = k;
    if(static_cast<int>(L_set.size()) < k) {
        my_k = L_set.size();
    }
    set<Node*> temp;
    for(int i = 0; i < my_k; i++) {
        struct info item = vector_info[i];
        temp.insert(item.node);
    }
    L_set = temp;

    return;
}
