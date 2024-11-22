#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <queue>
#include <Windows.h> // For Sleep function on Windows
#include <limits>
#include <algorithm>

using namespace std;

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
    string signalState; // Added to track signal state on individual roads

    Road(int id = -1, int start = -1, int end = -1, int length = 0, int capacity = 0)
        : id(id), start(start), end(end), length(length), capacity(capacity), currentCars(0), signalState("Red") {}
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

    RoadNetwork(int numIntersections) : numIntersections(numIntersections), running(true)
    {
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

    void addSignal(int from, int to){
        bool found = false;
        for (auto &road : adjList[from])
        {
            if (road.end == to)
            {
                road.signalState = "Red"; // Initialize signal state on this road
                found = true;
                intersections[from].hasSignal = true; // Marking the intersection as having a signal
                cout << "Signal added on road from " << from << " to " << to << ".\n";
            }
        }
        if (!found)
        {
            cout << "No road found from " << from << " to " << to << ".\n";
        }
    }

    void adjustSignalBasedOnTraffic(int intersectionId){
        int totalCars = 0;
        for (const Road &road : adjList[intersectionId])
        {
            totalCars += road.currentCars;
        }

        // Calculate traffic density as a percentage of capacity
        double density = (totalCars > 0) ? static_cast<double>(totalCars) / adjList[intersectionId].front().capacity : 0;

        // Adjust signal timing based on traffic density
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

    void manageSignals(){
        while (running)
        {
            for (int i = 0; i < numIntersections; ++i)
            {
                if (automaticSignals[i] && intersections[i].hasSignal)
                {
                    adjustSignalBasedOnTraffic(i);
                }
            }
            Sleep(5000);
        }
    }

    void addRoad(int roadId, int from, int to, int length, int capacity)
    {
        if (from < 0 || from >= numIntersections || to < 0 || to >= numIntersections)
        {
            cout << "Invalid intersection IDs. Please ensure the intersections exist.\n";
            return;
        }
        Road road(roadId, from, to, length, capacity);
        adjList[from].push_back(road);
        adjList[to].push_back(Road(roadId, to, from, length, capacity));
    }

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
        cout << "Unable to remove cars. Road does not exist or not enough cars.\n";
    }

    void toggleSignal(int intersectionId)
    {
        if (intersectionId >= 0 && intersectionId < numIntersections)
        {
            if (intersections[intersectionId].hasSignal)
            {
                intersections[intersectionId].toggleSignal();
                cout << "Signal at Intersection " << intersectionId << " toggled manually to "
                     << intersections[intersectionId].signalState << ".\n";
            }
            else
            {
                cout << "No signal installed at Intersection " << intersectionId << ".\n";
            }
        }
        else
        {
            cout << "Invalid intersection ID.\n";
        }
    }

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
        cout << "Unable to add cars. Road does not exist or capacity exceeded.\n";
    }

    void startAutomaticSignalControl(int intersectionId)
    {
        if (intersectionId >= 0 && intersectionId < numIntersections)
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

    void stopAutomaticSignalControl(int intersectionId)
    {
        if (intersectionId >= 0 && intersectionId < numIntersections)
        {
            if (!automaticSignals[intersectionId])
            {
                cout << "Automatic control is already inactive for intersection " << intersectionId << ".\n";
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

    void displayNetwork()
    {
        cout << "\n--- Traffic Network Status ---\n";
        for (int i = 0; i < numIntersections; ++i)
        {
            bool hasRoads = !adjList[i].empty();

            if (hasRoads)
            {
                for (const Road &road : adjList[i])
                {
                    cout << "Intersection " << i;
                    if (intersections[i].hasSignal)
                        cout << " [Signal: " << intersections[i].signalState << "]\n";
                    else
                        cout << " [No Signal]\n";

                    cout << "  -> Road " << road.id << " to Intersection " << road.end
                         << " (Length: " << road.length << "m, Capacity: " << road.capacity
                         << ", Current Cars: " << road.currentCars << ")\n";
                }
            }
        }
    }
};

void showMenu(RoadNetwork &network)
{
    while (true)
    {
        cout << "\n--- Traffic Control System Menu ---\n";
        cout << "1. Add Road\n";
        cout << "2. Remove Cars from Road\n";
        cout << "3. Add Signal\n";
        cout << "4. Toggle Signal\n";
        cout << "5. Add Cars to Road\n";
        cout << "6. Display Network Status\n";
        cout << "7. Start Automatic Signal\n";
        cout << "8. Stop Automatic Signal\n";
        cout << "0. Exit\n";
        cout << "Choose an option: ";

        int option = getValidatedInt("");

        switch (option)
        {
        case 1:
        {
            int id = getValidatedInt("Enter road ID: ");
            int start = getValidatedInt("Enter start intersection ID: ");
            int end = getValidatedInt("Enter end intersection ID: ");
            int length = getValidatedInt("Enter road length (m): ");
            int capacity = getValidatedInt("Enter road capacity: ");
            network.addRoad(id, start, end, length, capacity);
            break;
        }
        case 2:
        {
            int to = getValidatedInt("Enter road start intersection ID: ");
            int from = getValidatedInt("Enter road end intersection ID: ");
            int cars = getValidatedInt("Enter number of cars to transfer: ");
            network.removeCarsFromRoad(from, to, cars);
            break;
        }
        case 3:
        {
            int fromID = getValidatedInt("Enter intersection from ID to add a signal: ");
            int toID = getValidatedInt("Enter intersection end ID to add a signal: ");
            network.addSignal(fromID, toID);
            break;
        }
        case 4:
        {
            int intersectionId = getValidatedInt("Enter intersection ID to toggle the signal: ");
            network.toggleSignal(intersectionId);
            break;
        }
        case 5:
        {
            int from = getValidatedInt("Enter road start intersection ID: ");
            int to = getValidatedInt("Enter road end intersection ID: ");
            int cars = getValidatedInt("Enter number of cars to add: ");
            network.addCarsToRoad(from, to, cars);
            break;
        }
        case 6:
            network.displayNetwork();
            break;
        case 7: // Start Automatic Signal Control
        {
            int intersectionId = getValidatedInt("Enter intersection ID to start automatic signal control: ");
            network.startAutomaticSignalControl(intersectionId);
            break;
        }
        case 8: // Stop Automatic Signal Control
        {
            int intersectionId = getValidatedInt("Enter intersection ID to stop automatic signal control: ");
            network.stopAutomaticSignalControl(intersectionId);
            break;
        }
        case 0: // Exit
            cout << "Exiting the program...\n";
            return;
        default:
            cout << "Invalid option. Please try again.\n";
        }
    }
}

int main()
{
    int numIntersections;
    cout << "Enter number of intersections: ";
    cin >> numIntersections;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear the input buffer

    RoadNetwork network(numIntersections);
    showMenu(network);

    return 0;
}
