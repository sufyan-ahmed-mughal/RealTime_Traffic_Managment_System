#include "Add Road/AddRoadClass.h"
#include "Menu/Menu.h"
#include "Road Network/RoadNetworkClass.h" // Include the header file

using namespace std;

int main() {
    RoadNetwork network(100);
    int choice;
    while (true) {
        showMenu();
        cin >> choice;

        switch (choice) {
        case 1: {
            int roadId, from, to, length, capacity;
            cout << "Add Road Id: ";
            cin >> roadId;
            cout << "Road From Intersection: ";
            cin >> from;
            cout << "Road End Intersection: ";
            cin >> to;
            cout << "Road Length: ";
            cin >> length;
            cout << "Road Cars Capacity: ";
            cin >> capacity;
            network.addRoad(roadId, from, to, length, capacity);
            break;
        }
        default:
            cout << "Invalid option. Please try again.\n";
        }
    }
    
    return 0;
}
