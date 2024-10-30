// RoadNetwork.cpp
#include "RoadNetworkClass.h"

// Constructor implementation
RoadNetwork::RoadNetwork(int size) : adjList(size) {}

// Add road implementation
void RoadNetwork::addRoad(int roadId, int from, int to, int length, int capacity) {
    Road road(roadId, from, to, length, capacity);
    adjList[from].push_back(road);
    adjList[to].push_back(Road(roadId, to, from, length, capacity));
}
