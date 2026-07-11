#include <iostream>
#include <vector>
#include "CellNode.h"
#include "KDTree.h"
#include "MathUtils.h"
using namespace std;

// test for the KD-Tree + RF math using a random hand-built (prior to UI)
int main() {
    vector<CellNode> towers = {
        {"miami",        "LTE", 25.7617, -80.1918},
        {"orlando",      "LTE", 28.5383, -81.3792},
        {"jacksonville", "LTE", 30.3322, -81.6557},
        {"tampa",        "LTE", 27.9506, -82.4572}
    };

    KDTree tree;
    for (const CellNode& t : towers) tree.insert(t);

    struct Query { string label; double lat; double lon; string expected; };
    vector<Query> queries = {
        {"near Miami",        25.80, -80.20, "miami"},
        {"near Orlando",      28.50, -81.40, "orlando"},
        {"near Jacksonville", 30.30, -81.70, "jacksonville"},
        {"near Tampa",        27.90, -82.50, "tampa"}
    };

    cout << "=== KD-Tree nearest-neighbor test ===\n";
    int passed = 0;
    for (const Query& q : queries) {
        CellNode* result = tree.nearestNeighbor(q.lat, q.lon);
        if (result == nullptr) {
            cout << "[FAIL] " << q.label << ": returned nullptr\n";
            continue;
        }
        double dist = MathUtils::haversineDistance(q.lat, q.lon,
                                                   result->lat, result->lon);
        double signal = MathUtils::estimateSignalStrength(dist);
        bool ok = (result->cellID == q.expected);
        cout << (ok ? "[PASS] " : "[FAIL] ") << q.label
             << " -> " << result->cellID
             << "  (dist " << dist << " km, signal " << signal << " dBm)\n";
        if (ok) passed++;
    }

    cout << "\n" << passed << "/" << queries.size() << " queries correct\n";
    return (passed == (int)queries.size()) ? 0 : 1;
}
