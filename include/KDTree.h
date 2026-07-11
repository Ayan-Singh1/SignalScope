#pragma once
#include "CellNode.h"
struct KDNode {
    CellNode data;
    KDNode* left;
    KDNode* right;

    KDNode(const CellNode& node)
        : data(node), left(nullptr), right(nullptr) {}
};

// 2D KD-Tree over (latitude, longitude) - split axis alternates by depth
class KDTree {
private:
    KDNode* root;

    void insertRec(KDNode*& node, const CellNode& cell, int depth);
    void nearestRec(KDNode* node, double lat, double lon, int depth,
                    CellNode*& best, double& bestDist) const;
    void destroyRec(KDNode* node);

public:
    KDTree();
    ~KDTree();

    void insert(const CellNode& node);
    CellNode* nearestNeighbor(double lat, double lon);
};
