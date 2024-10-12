#ifndef HEADER_HPP
#define HEADER_HPP

#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <set>

using namespace std;

class Point {
    private:
        int id;
        vector<double> syntetagmenes;
        list<Point> edges;

    public:
        Point(int id, vector<double> syntetagmenes, list<Point> edges) {
            this->id = id;
            this->syntetagmenes = syntetagmenes;
            this->edges = edges;
        } // constructor

        int get_id() {
            return id;
        }
};

class Graph {
    private:
        map<int, Point> adj_list;

    public:
        void insert(Point a) {
            adj_list.insert(a.get_id(), a);
            adj_list[a.get_id()];
        }

};

#endif