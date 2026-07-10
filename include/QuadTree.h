#pragma once
#include <string>
#include <vector>
#include "CellNode.h"
using namespace std;

struct Boundary {
    double minX; // West Longitude
    double maxX; // East Longitude
    double minY; // South Latitude
    double maxY; // North Latitude

    bool contains(const CellNode& node) const;
    double midX() const; // Middle longitude of total boundary
    double midY() const; //Middle latitude of total boundary
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

public:
    QuadTree(const Boundary& boundary, int capacity);
    ~QuadTree();

    bool insert(const CellNode& node);

    CellNode* nearestNeighbor(double lat, double lon);

    void printBFS() const;
    

};