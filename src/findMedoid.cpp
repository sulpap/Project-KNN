#include "../include/graph.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <limits>
#include <random>

vector<int> getRandomSample(vector<int>& Pf, int taph) 
{
    // Edge case: if the number of requested samples is greater than the available data
    if (taph >= Pf.size()) {
        return Pf; // Return all elements if we want more or equal samples than available
    }

    // Initialize random number generator --> this is faster than the default engine
    random_device rd;
    mt19937 gen(rd());

    // Shuffle Pf to get random sampling
    shuffle(Pf.begin(), Pf.end(), gen);

    // Get the first 'taph' elements as the sampled result
    vector<int> Rf(Pf.begin(), Pf.begin() + taph);

    return Rf;
}

// p* <- arg min_{p in Rf} T[p]
int findMinInT(const vector<int>& Rf, const map<int, int>& T) 
{
    int p_star = -1;
    int min_value = numeric_limits<int>::max();

    // Iterate through each point in Rf
    for (const int& p : Rf) 
    {
        if (T.at(p) < min_value) { // Access T[p] safely using .at()
            min_value = T.at(p);
            p_star = p;
        }
    }

    return p_star;
}

map<int, Node*> FindMedoid(Graph &graph, int taph, set<int> F)
{
    map<int, Node *> M;
    map<int, int> T;

    // TODO is this needed? or is it automatically initialized to 0???
    for (int i = 0; i < 10; i++)
    {
        T[i] = 0;
    }

    // For each label in F
    for(auto& f : F)
    {
        // Find ids of all nodes with label f
        vector<int> Pf = graph.findNodesWithLabel(f);

        // Get taph randomly sampled data point ids from Pf
        vector<int> Rf = getRandomSample(Pf, taph); // taph is a threshold = an upper bound

        // p* <- arg min_{p in Rf} T[p]
        int p_star = findMinInT(Rf, T);

        // Update M[f] <- p* and T[p*] <- T[p*] + 1
        M[f] = graph.getNode(p_star);
        T[p_star] += 1; // T[p_star]++;
    }
    return M;
}