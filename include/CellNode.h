#pragma once
#include <string>
using namespace std;

struct CellNode {
    string cellID; // Unique node identifier from dataset
    string radioType; // LTE, 5G, GSM, etc.
    double lat; // Latitude (X-Cord)
    double lon; // Longitude (Y-Cord)
    double signalStrength = 0.0;
};