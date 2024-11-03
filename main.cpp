#include <iostream>
#include <vector>

using namespace std;

// R O A D - C L A S S
class Road
{
public:
    int id;
    int start;
    int end;
    int length;
    int capacity;
    int currentCars;

    Road(int id = -1, int start = -1, int end = -1, int length = 0, int capacity = 0)
    {
        this->id = id;
        this->start = start;
        this->end = end;
        this->length = length;
        this->capacity = capacity;
        this->currentCars = 0;
    }
};

// M A I N - C L A S S
class RoadNetwork
{
public:
    int numIntersections;
    vector<vector<Road>> adjList; // Adjacency list for roads

    RoadNetwork(int numIntersections) : numIntersections(numIntersections)
    {
        adjList.resize(numIntersections);
    }

    ~RoadNetwork()
    {
    }

    // Function for Adding Road
    void addRoad(int roadId, int from, int to, int length, int capacity)
    {
        Road road(roadId, from, to, length, capacity);
        adjList[from].push_back(road);
        adjList[to].push_back(Road(roadId, to, from, length, capacity));
    }
};

// M E N U
void showMenu()
{
    cout << "\n--- Traffic Management Menu ---\n";
    cout << "1. Add Road\n";
    cout << "Choose an option: ";
}

int main()
{
    int numIntersections;
    cout << "Enter the number of intersections in the road network: ";
    cin >> numIntersections;
    RoadNetwork network(numIntersections); // Initialize network with number of intersections

    int choice; // Variable for getting user choice

    while (true)
    {
        showMenu(); // Call the menu function
        cin >> choice;

        switch (choice)
        {
        case 1:
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
            cout << "Road added successfully.\n";
            break;
        default:
            cout << "Invalid option. Please try again.\n";
        }
    }

    return 0;
}
