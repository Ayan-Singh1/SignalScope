#include <iostream>
#include "QuadTree.h"
#include "CellNode.h"

using namespace std;

int main() {
    // Florida rough boundary
    // x = longitude, y = latitude
    Boundary floridaBoundary {
        -87.8, // minX: west longitude
        -79.8, // maxX: east longitude
        24.0,  // minY: south latitude
        31.1   // maxY: north latitude
    };

    // Create QuadTree with capacity 2 so it subdivides quickly during testing
    QuadTree tree(floridaBoundary, 2);

    // Test cell nodes
    CellNode miami;
    miami.cellID = "1";
    miami.radioType = "LTE";
    miami.lat = 25.7617;
    miami.lon = -80.1918;
    miami.signalStrength = -85;

    CellNode orlando;
    orlando.cellID = "2";
    orlando.radioType = "5G";
    orlando.lat = 28.5383;
    orlando.lon = -81.3792;
    orlando.signalStrength = -90;

    CellNode gainesville;
    gainesville.cellID = "3";
    gainesville.radioType = "LTE";
    gainesville.lat = 29.6516;
    gainesville.lon = -82.3248;
    gainesville.signalStrength = -92;

    CellNode tampa;
    tampa.cellID = "4";
    tampa.radioType = "LTE";
    tampa.lat = 27.9506;
    tampa.lon = -82.4572;
    tampa.signalStrength = -88;

    // Insert nodes
    cout << "Inserting nodes..." << endl;

    cout << "Miami inserted: " << tree.insert(miami) << endl;
    cout << "Orlando inserted: " << tree.insert(orlando) << endl;
    cout << "Gainesville inserted: " << tree.insert(gainesville) << endl;
    cout << "Tampa inserted: " << tree.insert(tampa) << endl;

    // Query near Gainesville
    double userLat = 29.65;
    double userLon = -82.32;

    CellNode* nearest = tree.nearestNeighbor(userLat, userLon);

    cout << endl;
    cout << "User location: (" << userLat << ", " << userLon << ")" << endl;

    if (nearest != nullptr) {
        cout << "Nearest node found:" << endl;
        cout << "Cell ID: " << nearest->cellID << endl;
        cout << "Radio Type: " << nearest->radioType << endl;
        cout << "Location: (" << nearest->lat << ", " << nearest->lon << ")" << endl;
        cout << "Signal Strength: " << nearest->signalStrength << " dBm" << endl;
    } else {
        cout << "No nearest node found." << endl;
    }

    return 0;
}