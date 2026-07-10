#include "KDTree.h"
#include "MathUtils.h"
#include <cfloat>   // DBL_MAX

KDTree::KDTree() : root(nullptr) {}

KDTree::~KDTree() {
    destroyRec(root);
}

// Post-order traversal freeing every node.
void KDTree::destroyRec(KDNode* node) {
    if (node == nullptr) return;
    destroyRec(node->left);
    destroyRec(node->right);
    delete node;
}

void KDTree::insert(const CellNode& node) {
    insertRec(root, node, 0);
}

// Recursive insert. Alternates axis by depth:
// even depth -> compare latitude, odd depth -> compare longitude.
void KDTree::insertRec(KDNode*& node, const CellNode& cell, int depth) {
    if (node == nullptr) {
        node = new KDNode(cell);
        return;
    }

    int axis = depth % 2; // 0 = latitude, 1 = longitude
    bool goLeft = (axis == 0) ? (cell.lat < node->data.lat)
                              : (cell.lon < node->data.lon);

    if (goLeft) {
        insertRec(node->left, cell, depth + 1);
    } else {
        insertRec(node->right, cell, depth + 1);
    }
}

CellNode* KDTree::nearestNeighbor(double lat, double lon) {
    if (root == nullptr) return nullptr;
    CellNode* best = nullptr;
    double bestDist = DBL_MAX;
    nearestRec(root, lat, lon, 0, best, bestDist);
    return best;
}

// Recursive nearest-neighbor with pruning: search the near side of the
// split first, then only cross to the far side if the splitting plane is
// closer than our best-so-far (meaning a closer point could still be there).
void KDTree::nearestRec(KDNode* node, double lat, double lon, int depth,
                        CellNode*& best, double& bestDist) const {
    if (node == nullptr) return;

    double d = MathUtils::haversineDistance(lat, lon,
                                            node->data.lat, node->data.lon);
    if (d < bestDist) {
        bestDist = d;
        best = &node->data;
    }

    int axis = depth % 2;
    KDNode* nearSide;
    KDNode* farSide;
    double planeDist;

    if (axis == 0) { // split on latitude
        if (lat < node->data.lat) { nearSide = node->left;  farSide = node->right; }
        else                      { nearSide = node->right; farSide = node->left;  }
        // Perpendicular distance to the latitude split line.
        planeDist = MathUtils::haversineDistance(lat, lon, node->data.lat, lon);
    } else {          // split on longitude
        if (lon < node->data.lon) { nearSide = node->left;  farSide = node->right; }
        else                      { nearSide = node->right; farSide = node->left;  }
        // Perpendicular distance to the longitude split line.
        planeDist = MathUtils::haversineDistance(lat, lon, lat, node->data.lon);
    }

    nearestRec(nearSide, lat, lon, depth + 1, best, bestDist);

    if (planeDist < bestDist) {
        nearestRec(farSide, lat, lon, depth + 1, best, bestDist);
    }
}
