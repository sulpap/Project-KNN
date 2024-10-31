#include "../include/greedysearch.hpp"
#include "../include/utility.hpp"
#include <algorithm>

#include <cassert>
#include <cfloat>   // due to DBL_MAX

struct info {
    Node* node;
    double distance;
};

static bool info_compare(const struct info &item_1, const struct info &item_2) {
    return item_1.distance < item_2.distance;
}

void GreedySearch(Graph &graph, Node* start_node, vector<double> &queryCoords, int k, int L, set<Node*> &L_set, set<Node*> &V_set) {

    // All the elements in a set have unique values
    // by default set is sorted in ascending order
    assert(L_set.empty() == true);      // given L_set should be empty
    assert(V_set.empty() == true);      // given V_set should be empty
    assert(L >= k);
    assert(start_node != NULL);
    assert(queryCoords.size() > 0);

    L_set.insert(start_node);  // Initialization of L_set
    // V_set is empty

    set<Node*> LminusV;               // initilize difference set to store difference of L\V
    LminusV.insert(start_node);    // L\V = {start} \ {} = {start}

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
        V_set.insert(p_star);       //  if p_star is already in V_set, p_set won't be inserted in V
        
        // Update L_set [ L = L U Nout(p*) ]
        list<Node*> p_star_out = p_star->getEdges();            // Οι εξερχόμενοι γείτονες του p*
        L_set.insert(p_star_out.begin(), p_star_out.end());

        // We cast L to long unsigned int, since that's what set.size returns
        if(L_set.size() > static_cast<set<Node*>::size_type>(L)) {                  // Update L to retain closests L points to query
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
    return;
}


// [1] Is there a possibility for an endless while?
// [2] Θεωρούμε ότι το query ταυτίζεται με κάποιο σημείο του γράφου?