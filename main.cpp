#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <Windows.h> // For Sleep function on Windows
#include <limits>

using namespace std;

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
};

class Road
{
public:
    int id;
    int start;
    int end;
    int length;
    int capacity;
    int currentCars;
    string signalState;

    Road(int id = -1, int start = -1, int end = -1, int length = 0, int capacity = 0)
        : id(id), start(start), end(end), length(length), capacity(capacity), currentCars(0), signalState("Red") {}
};

class RoadNetwork
{
public:
    unordered_map<int, Signal_Intersection> intersections;
    unordered_map<int, vector<Road>> adjList;
    unordered_set<int> roadIds;
    vector<bool> automaticSignals;
    thread signalThread;
    bool running;

    RoadNetwork(int numIntersections) : running(true), automaticSignals(numIntersections, false)
    {
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

    // Add Signal to the road with default red signal at the "to" intersection
    void addSignal(int fromIntersectionId, int toIntersectionId)
    {
        if (intersections.find(toIntersectionId) == intersections.end())
        {
            cout << "Intersection ID " << toIntersectionId << " does not exist.\n";
            return;
        }

        if (intersections[toIntersectionId].hasSignal)
        {
            cout << "Signal already installed at Intersection " << toIntersectionId << ".\n";
            return;
        }

        intersections[toIntersectionId].hasSignal = true;
        intersections[toIntersectionId].signalState = "Red";  // Default signal is Red

        for (auto &road : adjList[fromIntersectionId])
        {
            if (road.end == toIntersectionId)
            {
                road.signalState = "Red";  // Initialize the signal to Red for this road
                cout << "Signal installed at Intersection " << toIntersectionId << " for Road " << road.id
                     << " to Intersection " << road.end << " (Red).\n";
            }
        }
    }

    // Adjust the signal based on traffic density
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

    // This thread manages the signal updates every 5 seconds
    void manageSignals()
    {
        while (running)
        {
            for (auto &[id, intersection] : intersections)
            {
                if (intersection.hasSignal && automaticSignals[id])
                {
                    adjustSignalBasedOnTraffic(id);
                }
            }
            Sleep(5000); // Update every 5 seconds
        }
    }

    // Start automatic control for the signal at a particular intersection
    void startAutomaticSignalControl(int intersectionId)
    {
        if (intersectionId >= 0 && intersectionId < automaticSignals.size())
        {
            if (automaticSignals[intersectionId])
            {
                cout << "Automatic control is already active for intersection " << intersectionId << ".\n";
                return;
            }
            automaticSignals[intersectionId] = true;
            cout << "Automatic signal control started for intersection " << intersectionId << ".\n";
        }
        else
        {
            cout << "Invalid intersection ID.\n";
        }
    }

    // Stop automatic control for the signal at a particular intersection, reverting to manual mode
    void stopAutomaticSignalControl(int intersectionId)
    {
        if (intersectionId >= 0 && intersectionId < automaticSignals.size())
        {
            if (!automaticSignals[intersectionId])
            {
                cout << "Automatic control is not active for intersection " << intersectionId << ".\n";
                return;
            }
            automaticSignals[intersectionId] = false;
            cout << "Automatic signal control stopped for intersection " << intersectionId << ".\n";
        }
        else
        {
            cout << "Invalid intersection ID.\n";
        }
    }

    // Toggle the signal manually
    void toggleSignal(int intersectionId)
    {
        if (intersections.find(intersectionId) != intersections.end())
        {
            if (intersections[intersectionId].hasSignal)
            {
                for (auto &road : adjList[intersectionId])
                {
                    road.signalState = (road.signalState == "Red") ? "Green" : "Red";
                    cout << "Signal at road " << road.id << " toggled manually to " << road.signalState << ".\n";
                }
            }
            else
            {
                cout << "No signal installed at Intersection " << intersectionId << ".\n";
            }
        }
        else
        {
            cout << "Intersection ID " << intersectionId << " does not exist.\n";
        }
    }

    void addRoad(int roadId, int from, int to, int length, int capacity, bool isTwoWay)
    {
        if (roadIds.find(roadId) != roadIds.end())
        {
            cout << "Duplicate road ID detected. Road ID must be unique.\n";
            return;
        }

        if (intersections.find(from) == intersections.end())
        {
            intersections[from] = Signal_Intersection(from);
        }
        if (intersections.find(to) == intersections.end())
        {
            intersections[to] = Signal_Intersection(to);
        }

        for (const auto &road : adjList[from])
        {
            if (road.end == to)
            {
                cout << "Road from Intersection " << from << " to Intersection " << to << " already exists.\n";
                return;
            }
        }

        if (isTwoWay)
        {
            for (const auto &road : adjList[to])
            {
                if (road.end == from)
                {
                    cout << "Reverse road from Intersection " << to << " to Intersection " << from
                         << " already exists (Two-way road previously added).\n";
                    return;
                }
            }
        }

        Road road(roadId, from, to, length, capacity);
        adjList[from].push_back(road);
        roadIds.insert(roadId);

        if (isTwoWay)
        {
            Road reverseRoad(roadId, to, from, length, capacity);
            adjList[to].push_back(reverseRoad);
        }

        cout << "Road added from Intersection " << from << " to Intersection " << to;
        cout << (isTwoWay ? " (Two-way)." : " (One-way).") << "\n";
    }

    // Add Cars to Road
    void addCarsToRoad(int from, int to, int cars)
    {
        for (auto &road : adjList[from])
        {
            if (road.end == to && road.currentCars + cars <= road.capacity)
            {
                road.currentCars += cars;
                cout << cars << " cars added to road " << road.id << ".\n";
                return;
            }
        }
        cout << "Unable to add cars. Road does not exist in this direction or capacity exceeded.\n";
    }

    // Remove Cars from Road
    void removeCarsFromRoad(int from, int to, int cars)
    {
        for (auto &road : adjList[from])
        {
            if (road.end == to && road.currentCars >= cars)
            {
                road.currentCars -= cars;
                cout << cars << " cars removed from road " << road.id << ".\n";
                return;
            }
        }
        cout << "Unable to remove cars. Road does not exist in this direction or not enough cars on road.\n";
    }

    void displayNetwork()
    {
        cout << "\nIntersections:\n";
        for (const auto &[id, intersection] : intersections)
        {
            cout << "Intersection " << id << " -> Signal: " << (intersection.hasSignal ? intersection.signalState : "None") << "\n";
        }

        for (const auto &[from, roads] : adjList)
        {
            for (const auto &road : roads)
            {
                cout << "Road " << road.id << " from Intersection " << road.start << " to Intersection " << road.end
                     << " with Signal: " << road.signalState
                     << " | Cars: " << road.currentCars << "/" << road.capacity << "\n";
            }
        }
    }
};

int main()
{
    RoadNetwork network(5);

    // User will input the intersection IDs and the road details.
    while (true)
    {
        cout << "\n1. Add Road\n2. Add Signal\n3. Manage Signals\n4. Add Cars\n5. Remove Cars\n6. Display Network\n7. Exit\n";
        int choice = getValidatedInt("Enter your choice: ");

        if (choice == 1)
        {
            int roadId = getValidatedInt("Enter road ID: ");
            int from = getValidatedInt("Enter from Intersection ID: ");
            int to = getValidatedInt("Enter to Intersection ID: ");
            int length = getValidatedInt("Enter road length: ");
            int capacity = getValidatedInt("Enter road capacity: ");
            bool isTwoWay = getValidatedInt("Is this a two-way road (1 for Yes, 0 for No)? ") == 1;

            network.addRoad(roadId, from, to, length, capacity, isTwoWay);
        }
        else if (choice == 2)
        {
            int from = getValidatedInt("Enter from Intersection ID: ");
            int to = getValidatedInt("Enter to Intersection ID: ");
            network.addSignal(from, to);
        }
        else if (choice == 3)
        {
            int intersectionId = getValidatedInt("Enter intersection ID to manage: ");
            cout << "1. Start automatic control\n2. Stop automatic control\n3. Toggle signal\n";
            int action = getValidatedInt("Enter action: ");
            if (action == 1)
                network.startAutomaticSignalControl(intersectionId);
            else if (action == 2)
                network.stopAutomaticSignalControl(intersectionId);
            else if (action == 3)
                network.toggleSignal(intersectionId);
        }
        else if (choice == 4)
        {
            int from = getValidatedInt("Enter from Intersection ID: ");
            int to = getValidatedInt("Enter to Intersection ID: ");
            int cars = getValidatedInt("Enter number of cars to add: ");
            network.addCarsToRoad(from, to, cars);
        }
        else if (choice == 5)
        {
            int from = getValidatedInt("Enter from Intersection ID: ");
            int to = getValidatedInt("Enter to Intersection ID: ");
            int cars = getValidatedInt("Enter number of cars to remove: ");
            network.removeCarsFromRoad(from, to, cars);
        }
        else if (choice == 6)
        {
            network.displayNetwork();
        }
        else if (choice == 7)
        {
            break;
        }
        else
        {
            cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}
