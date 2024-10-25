#include "../include/greedysearch.hpp"
#include "../include/utility.hpp"
#include <algorithm>

// // start point
// // se kathe vhma koitaw geitones
// // ypologizw apostash apo query
// // epilegw closest geitona
// // ksana

// // GreedySearch returns node IDs !! not coords. so it's type vector<int> not vector<double>

// // we start from the startNode and move through the graph to find the closest nodes to the query.

// //if we have many unconnected graphs, we use graphUnion to combine them and then we do GreedySearch.

// vector<int> GreedySearch(Graph &graph, int startNodeId, vector<double> &queryCoords, int k) 
// {
//     cout << "Initiating GreedySearch..." <<endl; //debugging

//     vector<int> visitedNodes;
//     vector<int> result;

//     Node* currentNode = graph.getNode(startNodeId);

//     if (!currentNode) {
//         cout << "Start node does not exist in the graph." << endl;
//         return result;
//     }

//     visitedNodes.push_back(currentNode->getId());

//     // claculate the distance of our current node (startNode) and the query and store it
//     double closestDistance = euclidean_distance(currentNode->getCoordinates(), queryCoords);
//     int closestNodeId = currentNode->getId();


//     // compare distances to the closestDistance in order to find the k closest nodes
    












//     return result;
// }


#include <cassert>
#include <cfloat>   // due to DBL_MAX

void GreedySearch(Graph &graph, int startNodeId, vector<double> &queryCoords, int k, int L, set<int> &L_set, set<int> &V_set) {

    // All the elements in a set have unique values
    // by default set is sorted in ascending order
    assert(L_set.empty() == true);      // given L_set should be empty
    assert(V_set.empty() == true);      // given V_set should be empty
    assert(L >= k);

    L_set.insert(startNodeId);  // Initialization of L_set
    // V_set is empty

    set<int> LminusV;               // initilize difference set to store difference of L\V
    LminusV.insert(startNodeId);    // L\V = {start} \ {} = {start}

    while(LminusV.empty() == false) {       // while LminusV != {}        

        double min_dist = DBL_MAX;
        int min_id = -1;

        // We traverse LminusV:
        for (auto id : LminusV) {
            Node* node = graph.getNode(id); 
            vector<double> node_coord = node->getCoordinates();
            double distance = euclidean_distance(node_coord, queryCoords);
            if(distance < min_dist) {
                min_dist = distance;
                min_id = id;
            }
        }  
        p_star = min_id;

        // Update V_set [V = V U p*]
        V_set.insert(p_star);       //  if p_star is already in V_set, p_set won't be inserted in V
        
        // Update L_set [ L = L U Nout(p*) ]
        Node* p_star_node = graph.getNode(p_star);
        vector<int> p_star_out = p_star_node->getNeighbors();   // Περιέχει τα ids των γειτόνων του p*
        L_set.insert(p_star_out.begin(), p_star_out.end());

        if(L_set.size() > L) {
            // [TODO]
        }

        set<int> temp;
        set_difference(L_set.begin(), L_set.end(), V_set.begin(), V_set.end(), inserter(temp, temp.begin()));
        LminusV = temp;
    }
    return;
}


// [1] Is there a possibility for an endless while?
// [2] Θεωρούμε ότι το query ταυτίζεται με κάποιο σημείο του γράφου?