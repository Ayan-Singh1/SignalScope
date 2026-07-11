#pragma once
#include <string>
using namespace std;

struct CellNode {
    string cellID; // unique node identifier from dataset
    string radioType; // LTE, 5G, GSM, etc
    double lat; // lat (X-Cord)
    double lon; // long (Y-Cord)
    double signalStrength = 0.0;
};