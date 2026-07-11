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

    if(!divided && nodes.size() < capacity) { 
        nodes.push_back(node);
        return true;
    }

    if(!divided) {
        // --- ADD THIS SECURITY CHECK TO PREVENT INFINITE LOOPS ---
        // If all existing nodes have the exact same coordinate as the new node,
        // subdividing won't separate them. Just store it here instead!
        bool allDuplicates = true;
        for (const auto& existingNode : nodes) {
            if (existingNode.lat != node.lat || existingNode.lon != node.lon) {
                allDuplicates = false;
                break;
            }
        }
        
        if (allDuplicates) {
            nodes.push_back(node);
            return true;
        }
        // --------------------------------------------------------

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

    return false; // Safely catches any misses
}

// Nearest Neighbor helper functions
double distanceSquared(double lat1, double lon1, double lat2, double lon2) { // Used to compare closest nodes
    double dLat = lat1 - lat2;
    double dLon = lon1 - lon2;
    return dLat * dLat + dLon * dLon;
}

double Boundary::distanceSquaredTo(double lat, double lon) const { // Decide whether quadrant is worth searching (avoids brute force approach)
    double x = lon;
    double y = lat;

    double closestX = x;
    double closestY = y;

    if(x < minX) closestX = minX;
    else if (x > maxX) closestX = maxX;

    if(y < minY) closestY = minY;
    else if (y > maxY) closestY = maxY;

    double dx = x - closestX;
    double dy = y - closestY;

    return dx * dx + dy * dy;
}

void QuadTree::nearestNeighborHelper(double lat, double lon, CellNode*& best, double& bestDistance) {
    // If entire region is farther than best node, skip it
    if(boundary.distanceSquaredTo(lat, lon) > bestDistance) {
        return;
    }

    // Check nodes store in region
    for(CellNode& node : nodes) {
        double dist = distanceSquared(lat, lon, node.lat, node.lon);
        if(dist < bestDistance) {
            bestDistance = dist; // New best distance
            best = &node; // New best node
        }
    }

    if(divided) { // If region has children, search them as well
        northeast->nearestNeighborHelper(lat, lon, best, bestDistance);
        northwest->nearestNeighborHelper(lat, lon, best, bestDistance);
        southwest->nearestNeighborHelper(lat, lon, best, bestDistance);
        southeast->nearestNeighborHelper(lat, lon, best, bestDistance);
    }
}

CellNode* QuadTree::nearestNeighbor(double lat, double lon) {
    CellNode* best = nullptr;
    double bestDistance = 999999999.0; // Any real distance calculated will  be smaller (for testing)

    nearestNeighborHelper(lat, lon, best, bestDistance);
    return best;
}