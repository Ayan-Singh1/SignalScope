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

QuadTree::QuadTree(const Boundary& _boundary, int _capacity) {
    boundary = _boundary;
    capacity = _capacity;
    divided = false;
    northeast, northwest, southwest, southeast = nullptr;
}

QuadTree::~QuadTree() {
    delete northeast;
    delete northwest;
    delete southwest;
    delete southeast;
}