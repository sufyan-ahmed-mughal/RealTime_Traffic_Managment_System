#include <iostream>
#include <vector>
#include <string>

using namespace std;

// R O A D - C L A S S
class Road
{
public:
    int id, start, end, length, capacity, currentCars;
    string signalState;

    Road(int id = -1, int start = -1, int end = -1, int length = 0, int capacity = 0)
        : id(id), start(start), end(end), length(length), capacity(capacity), currentCars(0), signalState("None") {}
};

// I N T E R S E C T I O N - C L A S S
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

// M A I N - C L A S S
class RoadNetwork
{
public:
    int numIntersections;
    vector<Signal_Intersection> intersections;
    vector<vector<Road>> adjList;
    vector<bool> automaticSignals;

    RoadNetwork(int numIntersections) : numIntersections(numIntersections)
    {
        adjList.resize(numIntersections);
        intersections.resize(numIntersections);
        automaticSignals.resize(numIntersections, false);
        for (int i = 0; i < numIntersections; i++)
        {
            intersections[i] = Signal_Intersection(i);
        }
    }

    void addRoad(int roadId, int from, int to, int length, int capacity)
    {
        adjList[from].push_back(Road(roadId, from, to, length, capacity));
        adjList[to].push_back(Road(roadId, to, from, length, capacity)); // Bidirectional
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

    void addSignal(int from, int to)
    {
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

    void adjustSignalBasedOnTraffic(int intersectionId)
    {
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
};

// M E N U
void showMenu()
{
    cout << "\n--- Traffic Management Menu ---\n"
         << "1. Add Road\n"
         << "2. Display Road Network\n"
         << "3. Add Cars to Road\n"
         << "4. Remove Cars from Road\n"
         << "5. Add Signal\n"
         << "6. Toggle Signal\n"
         << "7. Start Automatic Signal\n"
         << "8. Stop Automatic Signal\n"
         << "0. Exit\n"
         << "Choose an option: ";
}

int main()
{
    int numIntersections;
    cout << "Enter the number of intersections in the road network: ";
    cin >> numIntersections;

    RoadNetwork network(numIntersections);
    int choice;

    while (true)
    {
        showMenu();
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            int roadId, from, to, length, capacity;
            cout << "Enter road ID: ";
            cin >> roadId;
            cout << "Enter start intersection: ";
            cin >> from;
            cout << "Enter end intersection: ";
            cin >> to;
            cout << "Enter road length: ";
            cin >> length;
            cout << "Enter road capacity: ";
            cin >> capacity;
            network.addRoad(roadId, from, to, length, capacity);
            break;
        }
        case 2:
            network.displayNetwork();
            break;
        case 3:
        {
            int from, to, cars;
            cout << "Enter start intersection: ";
            cin >> from;
            cout << "Enter end intersection: ";
            cin >> to;
            cout << "Enter cars you want to add: ";
            cin >> cars;
            network.addCarsToRoad(from, to, cars);
            break;
        }
        case 4:
        {
            int from, to, cars;
            cout << "Enter start intersection: ";
            cin >> from;
            cout << "Enter end intersection: ";
            cin >> to;
            cout << "Enter cars you want to remove: ";
            cin >> cars;
            network.removeCarsFromRoad(from, to, cars);
            break;
        }
        case 5:
        {
            int from, to;
            cout << "Enter start intersection: ";
            cin >> from;
            cout << "Enter end intersection: ";
            cin >> to;
            network.addSignal(from, to);
            break;
        }
        case 6:
        {
            int intersectionId;
            cout << "Enter intersection ID to toggle the signal: ";
            cin >> intersectionId;
            network.toggleSignal(intersectionId);
            break;
        }
        case 7:
        {
            int intersectionId;
            cout << "Enter intersection ID to start automatic signal control: ";
            cin >> intersectionId;
            network.startAutomaticSignalControl(intersectionId);
            break;
        }
        case 8:
        {
            int intersectionId;
            cout << "Enter intersection ID to stop automatic signal control: ";
            cin >> intersectionId;
            network.stopAutomaticSignalControl(intersectionId);
            break;
        }
        case 0:
        {
            exit(0);
        }
        default:
            cout << "Invalid choice. Try again.\n";
        }
    }
}
