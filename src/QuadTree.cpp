#include <string>
#include <vector>
#include <iostream>
#include "QuadTree.h"
using namespace std;

bool Boundary::contains(const CellNode& node) const {
    double x = node.lon;
    double y = node.lat;
    if((x >= minX && x < maxX) && (y >= minY && y < maxY)) { // Check if x, y cords are inside this boundary
        return true;
    } else {
        return false;
    }
}

double Boundary::midX() const {
    return((minX + maxX) / 2);
}

double Boundary::midY() const {
    return((minY + maxY) / 2);
}

QuadTree::QuadTree(const Boundary& boundary, int capacity) {
    this->boundary = boundary;
    this->capacity = capacity;
    this->divided = false;
    this->northeast = nullptr;
    this->northwest = nullptr; 
    this->southwest = nullptr;
    this->southeast = nullptr;
}

QuadTree::~QuadTree() {
    delete northeast;
    delete northwest;
    delete southwest;
    delete southeast;
}

void QuadTree::subDivide() {
    double midX = boundary.midX();
    double midY = boundary.midY();

    Boundary neBoundary { // New NE Boundary
        midX, boundary.maxX, // Left->Right
        midY, boundary.maxY // Bottom->Top
    };
    Boundary nwBoundary { // New NW Boundary
        boundary.minX, midX,
        midY, boundary.maxY
    };
    Boundary swBoundary { // New SW Boundary
        boundary.minX, midX,
        boundary.minY, midY
    };
    Boundary seBoundary { // New SE Boundary
        midX, boundary.maxX,
        boundary.minY, midY
    };

    northeast = new QuadTree(neBoundary, capacity);
    northwest = new QuadTree(nwBoundary, capacity);
    southwest = new QuadTree(swBoundary, capacity);
    southeast = new QuadTree(seBoundary, capacity);

    divided = true; // Updated QuadTree divided
}

bool QuadTree::insert(const CellNode& node) {
    if(!boundary.contains(node)) {
        return false;
    }

    if(!divided && nodes.size() < capacity) { // Region has space and is not divided, store node here
        nodes.push_back(node);
        return true;
    }

    if(!divided) {
        subDivide();
        vector<CellNode> oldNodes = nodes;
        nodes.clear();

        for(const CellNode& oldNode : oldNodes) {
            insert(oldNode);
        }
    }
    if(northeast->insert(node)) return true;
    if(northwest->insert(node)) return true;
    if(southwest->insert(node)) return true;
    if(southeast->insert(node)) return true;

}