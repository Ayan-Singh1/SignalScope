#pragma once
#include <string>
#include <vector>
#include "CellNode.h"
using namespace std;

// Standalone global helper function declaration
double distanceSquared(double lat1, double lon1, double lat2, double lon2);

struct Boundary {
    double minX; // West Longitude
    double maxX; // East Longitude
    double minY; // South Latitude
    double maxY; // North Latitude

    bool contains(const CellNode& node) const;
    double midX() const; // Middle longitude of total boundary
    double midY() const; // Middle latitude of total boundary

    double distanceSquaredTo(double lat, double lon) const;
};

class QuadTree {
private:
    Boundary boundary;
    int capacity;
    bool divided;
    vector<CellNode> nodes;

    // Child Order Convention (Cartesian Convention): NE, NW, SW, SE
    QuadTree* northeast;
    QuadTree* northwest;
    QuadTree* southwest;
    QuadTree* southeast;

    void subDivide();
    
    // Added the missing helper declaration!
    void nearestNeighborHelper(double lat, double lon, CellNode*& best, double& bestDistance);

public:
    QuadTree(const Boundary& boundary, int capacity);
    ~QuadTree();

    bool insert(const CellNode& node);

    CellNode* nearestNeighbor(double lat, double lon);

    void printBFS() const;
};