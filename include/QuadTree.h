#pragma once
#include <string>
#include <vector>
#include "CellNode.h"
using namespace std;
double distanceSquared(double lat1, double lon1, double lat2, double lon2);

struct Boundary {
    double minX; 
    double maxX;
    double minY;
    double maxY;

    bool contains(const CellNode& node) const;
    double midX() const;
    double midY() const;

    double distanceSquaredTo(double lat, double lon) const;
};

class QuadTree {
private:
    Boundary boundary;
    int capacity;
    bool divided;
    vector<CellNode> nodes;
    QuadTree* northeast;
    QuadTree* northwest;
    QuadTree* southwest;
    QuadTree* southeast;

    void subDivide();
    void nearestNeighborHelper(double lat, double lon, CellNode*& best, double& bestDistance);

public:
    QuadTree(const Boundary& boundary, int capacity);
    ~QuadTree();

    bool insert(const CellNode& node);

    CellNode* nearestNeighbor(double lat, double lon);

    void printBFS() const;
};