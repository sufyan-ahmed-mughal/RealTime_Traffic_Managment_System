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
        this->signalState = "None"; // Set default signal state to None
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
    string signalState; // Signal state for this road

    Road(int start = -1, int end = -1, int length = 0, int capacity = 0, bool isOneWay = false)
        : start(start), end(end), length(length), capacity(capacity), currentCars(0), isOneWay(isOneWay), signalState("None") {}
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
        numIntersections = 10000000;
        intersections.resize(numIntersections);
        adjList.resize(numIntersections);
        automaticSignals.resize(numIntersections, false); // Automatic signal control is not started by default
        for (int i = 0; i < numIntersections; ++i)
        {
            intersections[i] = Signal_Intersection(i); // All signals are set to "None" by default
        }
        signalThread = thread(&RoadNetwork::manageSignals, this); // Automatically manage signals in background
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
        bool roadFound = false;
        for (auto &road : adjList[from])
        {
            if (road.end == to)
            {
                roadFound = true;

                // Check if the road already has a signal
                if (road.signalState != "None")
                {
                    cout << "Signal already exists on road from Intersection " << from << " to Intersection " << to << ".\n";
                    return;
                }

                // Add signal to this specific road
                road.signalState = "Red";
                cout << "Signal added on road from Intersection " << from << " to Intersection " << to << ".\n";

                return;
            }
        }

        if (!roadFound)
        {
            cout << "No road found from Intersection " << from << " to Intersection " << to << ".\n";
        }
    }

    void startAutomaticSignalControl()
    {
        int from = getValidatedInt("Enter the starting intersection ID: ");
        int to = getValidatedInt("Enter the ending intersection ID: ");

        if (from >= 0 && to >= 0 && from < numIntersections && to < numIntersections)
        {
            automaticSignals[from] = true; // Activate automatic control for the intersection
            running = true;
            cout << "Automatic signal control started for Intersection " << from << " to " << to << ".\n";
        }
        else
        {
            cout << "Invalid intersection IDs.\n";
        }
    }

    void stopAutomaticSignalControl()
    {
        int from = getValidatedInt("Enter the starting intersection ID: ");
        int to = getValidatedInt("Enter the ending intersection ID: ");

        if (from >= 0 && to >= 0 && from < numIntersections && to < numIntersections)
        {
            automaticSignals[from] = false; // DeActivate automatic control for the intersection
            running = true;
            cout << "Automatic signal control started for Intersection " << from << " to " << to << ".\n";
        }
        else
        {
            cout << "Invalid intersection IDs.\n";
        }
    }
    void manageSignals()
    {
        while (running)
        {
            for (int i = 0; i < numIntersections; ++i)
            {
                if (automaticSignals[i]) // If automatic signal control is active for this intersection
                {
                    for (auto &road : adjList[i])
                    {
                        adjustSignalForRoad(road.start, road.end);
                    }
                }
            }
            this_thread::sleep_for(chrono::seconds(5)); // Sleep for 5 seconds before checking again
        }
    }

    void adjustSignalForRoad(int from, int to)
    {
        for (auto &road : adjList[from])
        {
            if (road.end == to)
            {
                double density = (road.capacity > 0) ? static_cast<double>(road.currentCars) / road.capacity : 0;

                // Define thresholds for managing green light cycles
                if (density > 0.75)
                {
                    road.signalState = "Green"; // Heavy traffic, set signal to green
                }
                else if (density > 0.50)
                {
                    road.signalState = "Yellow"; // Moderate traffic, set signal to yellow
                }
                else
                {
                    road.signalState = "Red"; // Low traffic, set signal to red
                }

                // Prioritize roads with higher density by giving them more green light time
                if (road.signalState == "Green")
                {
                    cout << "Signal on road from " << road.start << " to " << road.end << " is Green (Density: " << density << ").\n";
                    return;
                }
                else
                {
                    cout << "Signal on road from " << road.start << " to " << road.end << " is " << road.signalState << " (Density: " << density << ").\n";
                    return;
                }
            }
        }
        cout << "No road found from " << from << " to " << to << ".\n";
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

    void addRoad(int from, int to, int length, int capacity, bool isOneWay)
    {
        if (from < 0 || from >= numIntersections || to < 0 || to >= numIntersections)
        {
            cout << "Invalid intersection IDs.\n";
            return;
        }
        adjList[from].emplace_back(from, to, length, capacity, isOneWay);
        if (!isOneWay)
        {
            adjList[to].emplace_back(to, from, length, capacity, isOneWay);
        }
    }
        void printRoads()
    {
        for (int i = 0; i < numIntersections; ++i)
        {
            for (const auto &road : adjList[i])
            {
                cout << "Road from " << road.start << " to " << road.end
                     << ", Length: " << road.length
                     << "m, Capacity: " << road.capacity
                     << ", Current cars: " << road.currentCars
                     << ", One-way: " << (road.isOneWay ? "Yes" : "No")
                     << ", Signal: " << road.signalState << "\n";
            }
        }
    }

};

int main()
{
    RoadNetwork network;
    int choice;
    while (true)
    {
        cout << "\nMenu:\n";
        cout << "1. Add Signal to Road\n";
        cout << "2. Add Road\n";
        cout << "3. Add Cars\n";
        cout << "4. Remove Cars\n";
        cout << "5. Start Automatic Signal Control\n";
        cout << "6. Stop Automatic Signal Control\n";
        cout << "7. Find Shortest Path\n";
        cout << "8. Print Roads\n";
        cout << "9. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            int from = getValidatedInt("Enter the starting intersection ID: ");
            int to = getValidatedInt("Enter the ending intersection ID: ");
            network.addSignal(from, to);
            break;
        }
        case 2:
        {
            int from = getValidatedInt("Enter the starting intersection ID: ");
            int to = getValidatedInt("Enter the ending intersection ID: ");
            int length = getValidatedInt("Enter the road length in meters: ");
            int capacity = getValidatedInt("Enter the road capacity: ");
            bool isOneWay;
            cout << "Is the road one-way? (1 for Yes, 0 for No): ";
            cin >> isOneWay;
            network.addRoad(from, to, length, capacity, isOneWay);
            break;
        }
        case 3:
        {
            int from, to, carCount;
            from = getValidatedInt("Enter the starting intersection ID: ");
            to = getValidatedInt("Enter the ending intersection ID: ");
            carCount = getValidatedInt("Enter the number of cars to add: ");
            network.addCarsToRoad(from, to, carCount);
            break;
        }
                case 4:
        {
            int from, to, carCount;
            from = getValidatedInt("Enter the starting intersection ID: ");
            to = getValidatedInt("Enter the ending intersection ID: ");
            carCount = getValidatedInt("Enter the number of cars to remove: ");
            network.removeCarsFromRoad(from, to, carCount);
            break;
        }
        case 5:
        {
            network.startAutomaticSignalControl();
            break;
        }
        case 6:
        {
            network.stopAutomaticSignalControl();
            break;
        }
        case 7:
        {
            int start = getValidatedInt("Enter the starting intersection ID: ");
            int end = getValidatedInt("Enter the ending intersection ID: ");
            network.findShortestPath(start, end);
            break;
        }
        case 8:
        {
            network.printRoads();
            break;
        }
        case 9:
        {
            cout << "Exiting program...\n";
            return 0;
        }
        default:
        {
            cout << "Invalid choice, please try again.\n";
            break;
        }
        }
    }
}




