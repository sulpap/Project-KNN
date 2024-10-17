#include <../include/utility.hpp>
#include "../include/graph.hpp"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <cassert>

double euclideanDistance(vector<double> coords1, vector<double> coords2) 
{
    double sum = 0.0;

    for (auto i = 0.0; i < coords1.size(); ++i) {                                 // auto or smth else??
        sum += pow(coords2[i] - coords1[i], 2);
    }

    return sqrt(sum);
}

double calculate_eclideanDistance(Node* node1, Node* node2)
{
    //take the coords from the two nodes and call euclideanDistance
    return euclideanDistance(node1->getCoordinates(), node2->getCoordinates());
}