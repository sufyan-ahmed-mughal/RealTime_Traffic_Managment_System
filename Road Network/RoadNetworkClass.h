// RoadNetwork.h
#ifndef ROADNETWORK_H
#define ROADNETWORK_H

#include <iostream>
#include <vector>

class RoadNetwork {
public:
    std::vector<std::vector<Road>> adjList; // Adjacency list for roads

    // Constructor to initialize the adjList with a specific size
    RoadNetwork(int size); // Declaration of constructor

    void addRoad(int roadId, int from, int to, int length, int capacity); // Function declaration
};

#endif // ROADNETWORK_H
