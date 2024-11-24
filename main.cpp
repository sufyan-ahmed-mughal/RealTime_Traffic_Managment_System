#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <queue>
#include <Windows.h> // For Sleep function on Windows
#include <limits>
#include <algorithm>

using namespace std;

struct Node
{
    int id;
    int distance;

    bool operator>(const Node &other) const
    {
        return distance > other.distance;
    }
};

// Function to validate integer input
int getValidatedInt(const string &prompt)
{
    int value;
    while (true)
    {
        cout << prompt;
        cin >> value;
        if (cin.fail() || value < 0)
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a non-negative integer.\n";
        }
        else
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
    }
}

class Signal_Intersection
{
public:
    int id;
    bool hasSignal;
    string signalState;

    Signal_Intersection(int id = 0)
    {
        this->id = id;
        this->hasSignal = false;
        this->signalState = "Red";
    }

    void toggleSignal()
    {
        signalState = (signalState == "Red") ? "Green" : "Red";
    }

    void adjustSignal(int totalCars, int capacity)
    {
        double density = (capacity > 0) ? static_cast<double>(totalCars) / capacity : 0;

        if (density > 0.75)
        {
            signalState = "Green";
        }
        else if (density > 0.50)
        {
            signalState = "Yellow";
        }
        else
        {
            signalState = "Red";
        }
    }
};

class Road
{
public:
    int start;
    int end;
    int length;
    int capacity;
    int currentCars;
    bool isOneWay;

    Road(int start = -1, int end = -1, int length = 0, int capacity = 0, bool isOneWay = false)
        : start(start), end(end), length(length), capacity(capacity), currentCars(0), isOneWay(isOneWay) {}
};

class RoadNetwork
{
public:
    int numIntersections;
    vector<Signal_Intersection> intersections;
    vector<vector<Road>> adjList;
    vector<bool> automaticSignals;
    thread signalThread;
    bool running;

    RoadNetwork() : running(true)
    {
        numIntersections = getValidatedInt("Enter the number of intersections: ");
        intersections.resize(numIntersections);
        adjList.resize(numIntersections);
        automaticSignals.resize(numIntersections, false);
        for (int i = 0; i < numIntersections; ++i)
        {
            intersections[i] = Signal_Intersection(i);
        }
        signalThread = thread(&RoadNetwork::manageSignals, this);
    }

    ~RoadNetwork()
    {
        running = false;
        if (signalThread.joinable())
        {
            signalThread.join();
        }
    }

    void addSignal(int from, int to)
    {
        bool found = false;
        for (auto &road : adjList[from])
        {
            if (road.end == to)
            {
                found = true;
                intersections[from].hasSignal = true;
                cout << "Signal added on road from " << from << " to " << to << ".\n";
            }
        }
        if (!found)
        {
            cout << "No road found from " << from << " to " << to << ".\n";
        }
    }

    void adjustSignalBasedOnTraffic(int intersectionId)
    {
        int totalCars = 0;
        for (const Road &road : adjList[intersectionId])
        {
            totalCars += road.currentCars;
        }

        double density = (totalCars > 0) ? static_cast<double>(totalCars) / adjList[intersectionId].front().capacity : 0;

        if (density > 0.75)
        {
            intersections[intersectionId].signalState = "Green";
        }
        else if (density > 0.50)
        {
            intersections[intersectionId].signalState = "Yellow";
        }
        else
        {
            intersections[intersectionId].signalState = "Red";
        }
    }

    void findShortestPath(int start, int end)
    {
        vector<int> distances(numIntersections, INT_MAX);
        vector<int> previous(numIntersections, -1);
        priority_queue<Node, vector<Node>, greater<Node>> pq;

        distances[start] = 0;
        pq.push({start, 0});

        while (!pq.empty())
        {
            int current = pq.top().id;
            pq.pop();

            if (current == end)
            {
                break;
            }

            for (const Road &road : adjList[current])
            {
                int neighbor = road.end;
                int newDist = distances[current] + road.length;

                if (newDist < distances[neighbor])
                {
                    distances[neighbor] = newDist;
                    previous[neighbor] = current;
                    pq.push({neighbor, newDist});
                }
            }
        }

        if (distances[end] == INT_MAX)
        {
            cout << "No path found from Intersection " << start << " to Intersection " << end << ".\n";
            return;
        }

        cout << "Shortest path from Intersection " << start << " to Intersection " << end << " is: ";
        vector<int> path;
        for (int at = end; at != -1; at = previous[at])
        {
            path.push_back(at);
        }
        reverse(path.begin(), path.end());

        for (int id : path)
        {
            cout << id << (id == end ? "\n" : " -> ");
        }
        cout << "Total distance: " << distances[end] << "m\n";
    }

    void manageSignals()
    {
        while (running)
        {
            for (int i = 0; i < numIntersections; ++i)
            {
                if (automaticSignals[i] && intersections[i].hasSignal)
                {
                    adjustSignalBasedOnTraffic(i);
                }
            }
            Sleep(5000); // Sleep for 5 seconds before checking again
        }
    }

    void addRoad(int from, int to, int length, int capacity, bool isOneWay)
    {
        if (from < 0 || from >= numIntersections || to < 0 || to >= numIntersections)
        {
            cout << "Invalid intersection IDs.\n";
            return;
        }

        for (const auto &road : adjList[from])
        {
            if (road.end == to)
            {
                cout << "Road already exists.\n";
                return;
            }
        }

        if (!isOneWay)
        {
            for (const auto &road : adjList[to])
            {
                if (road.end == from)
                {
                    cout << "Bidirectional road exists.\n";
                    return;
                }
            }
        }

        Road road(from, to, length, capacity, isOneWay);
        adjList[from].push_back(road);

        if (!isOneWay)
        {
            adjList[to].push_back(Road(to, from, length, capacity, isOneWay));
        }

        cout << "Road added.\n";
    }

    void removeCarsFromRoad(int from, int to, int cars)
    {
        bool roadFound = false;
        for (auto &road : adjList[from])
        {
            if (road.end == to)
            {
                if (road.currentCars >= cars)
                {
                    road.currentCars -= cars;
                    cout << cars << " cars removed.\n";
                }
                else
                {
                    cout << "Not enough cars.\n";
                }
                roadFound = true;
                break;
            }
        }

        if (!roadFound)
        {
            cout << "No road found.\n";
        }
    }

    void transferCars(int from, int to, int cars)
    {
        bool fromRoadFound = false, toRoadFound = false;

        for (auto &road : adjList[from])
        {
            if (road.end == to)
            {
                if (road.currentCars >= cars)
                {
                    road.currentCars -= cars;
                    fromRoadFound = true;
                }
                else
                {
                    cout << "Not enough cars.\n";
                }
                break;
            }
        }

        if (fromRoadFound)
        {
            for (auto &road : adjList[to])
            {
                if (road.start == from && road.currentCars + cars <= road.capacity)
                {
                    road.currentCars += cars;
                    toRoadFound = true;
                    break;
                }
            }
        }

        if (!fromRoadFound)
        {
            cout << "No road found.\n";
        }
        else if (!toRoadFound)
        {
            cout << "Unable to add cars.\n";
        }
    }

    void addCarsToRoad(int from, int to, int cars)
    {
        bool roadFound = false;
        for (auto &road : adjList[from])
        {
            if (road.end == to)
            {
                if (road.currentCars + cars <= road.capacity)
                {
                    road.currentCars += cars;
                    cout << cars << " cars added.\n";
                    roadFound = true;
                }
                else
                {
                    cout << "Capacity exceeded.\n";
                }
                break;
            }
        }

        if (!roadFound)
        {
            cout << "No road found.\n";
        }
    }

void printRoads()
{
    for (int i = 0; i < numIntersections; ++i)
    {
        for (const auto &road : adjList[i])
        {
            // Print road details
            cout << "Road from " << road.start << " to " << road.end
                 << ", Length: " << road.length
                 << "m, Capacity: " << road.capacity
                 << ", Current cars: " << road.currentCars
                 << ", One-way: " << (road.isOneWay ? "Yes" : "No") << "\n";

            // Print signal state for the starting intersection
            if (intersections[road.start].hasSignal)
            {
                cout << "Signal at Intersection " << road.start << " is " << intersections[road.start].signalState << "\n";
            }

            // Print signal state for the ending intersection, especially if it's a one-way road
            if (road.isOneWay && intersections[road.end].hasSignal)
            {
                cout << "Signal at Intersection " << road.end << " is " << intersections[road.end].signalState << "\n";
            }
            // Print signal state for two-way roads as well
            else if (!road.isOneWay && intersections[road.end].hasSignal)
            {
                cout << "Signal at Intersection " << road.end << " is " << intersections[road.end].signalState << "\n";
            }
        }
    }
}

    void toggleSignal(int intersectionId)
    {
        intersections[intersectionId].toggleSignal();
        cout << "Signal at Intersection " << intersectionId << " is now " << intersections[intersectionId].signalState << ".\n";
    }

    void startAutomaticSignalControl(int intersectionId)
    {
        automaticSignals[intersectionId] = true;
        cout << "Automatic signal control started for Intersection " << intersectionId << ".\n";
    }

    void stopAutomaticSignalControl(int intersectionId)
    {
        automaticSignals[intersectionId] = false;
        cout << "Automatic signal control started for Intersection " << intersectionId << ".\n";
    }
};

int main()
{
    RoadNetwork network;
    bool running = true;

    while (running)
    {
        cout << "\nMenu:\n";
        cout << "1. Add Road\n";
        cout << "2. Add Signal\n";
        cout << "3. Find Shortest Path\n";
        cout << "4. Toggle Signal\n";
        cout << "5. Print Roads\n";
        cout << "6. Add Cars\n";
        cout << "7. Remove Cars\n";
        cout << "8. Start Automatic Signal\n";
        cout << "9. Stop Automatic Signal\n";
        cout << "10. Exit\n";
        cout << "Enter choice: ";
        int choice = getValidatedInt("");

        switch (choice)
        {
        case 1:
        {
            int from = getValidatedInt("Enter starting intersection: ");
            int to = getValidatedInt("Enter ending intersection: ");
            int length = getValidatedInt("Enter road length: ");
            int capacity = getValidatedInt("Enter road capacity: ");
            bool isOneWay = getValidatedInt("Is the road one-way? (1 for Yes, 0 for No): ");
            network.addRoad(from, to, length, capacity, isOneWay);
            break;
        }
        case 2:
        {
            int from = getValidatedInt("Enter starting intersection for signal: ");
            int to = getValidatedInt("Enter ending intersection for signal: ");
            network.addSignal(from, to);
            break;
        }
        case 3:
        {
            int start = getValidatedInt("Enter start intersection: ");
            int end = getValidatedInt("Enter end intersection: ");
            network.findShortestPath(start, end);
            break;
        }
        case 4:
        {
            int intersectionId = getValidatedInt("Enter intersection ID to toggle signal: ");
            network.toggleSignal(intersectionId);
            break;
        }
        case 5:
            network.printRoads();
            break;
        case 6:
        {
            int from = getValidatedInt("Enter start intersection: ");
            int to = getValidatedInt("Enter end intersection: ");
            int cars = getValidatedInt("Enter Cars you want to Add: ");
            network.addCarsToRoad(from, to, cars);
            break;
        }
        case 7:
        {
            int from = getValidatedInt("Enter start intersection: ");
            int to = getValidatedInt("Enter end intersection: ");
            int cars = getValidatedInt("Enter Cars you want to Remove: ");
            network.removeCarsFromRoad(from, to, cars);
            break;
        }
        case 8:
        {
            int intersectionId = getValidatedInt("Enter intersection ID to start automatic signal control: ");
            network.startAutomaticSignalControl(intersectionId);
            break;
        }
        case 9:
        {
            int intersectionId = getValidatedInt("Enter intersection ID to stop automatic signal control: ");
            network.stopAutomaticSignalControl(intersectionId);
            break;
        }
        case 10:
            running = false;
            cout << "Exiting program.\n";
            break;
        default:
            cout << "Invalid choice. Please try again.\n";
            break;
        }
    }

    return 0;
}
