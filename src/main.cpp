#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include <SFML/Graphics.hpp>

// Include teammate's code
#include "CellNode.h"
#include "QuadTree.h"
#include "KDTree.h"
#include "MathUtils.h"

using namespace std;

// Florida Map Boundaries (Used to map pixels to coordinates)
const double MIN_LON = -87.8;
const double MAX_LON = -79.8;
const double MIN_LAT = 24.0;
const double MAX_LAT = 31.1;

// Helper to load the CSV at runtime
void loadCSV(const string& filename, QuadTree& qt, KDTree& kdt) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open " << filename << "\n";
        return;
    }

    string line;
    getline(file, line); // Skip header row

    int count = 0;
    while (getline(file, line)) {
        stringstream ss(line);
        string field;
        vector<string> fields;

        while (getline(ss, field, ',')) {
            fields.push_back(field);
        }

        if (fields.size() >= 8) {
            try {
                CellNode node;
                node.radioType = fields[0];
                node.lon = stod(fields[6]);
                node.lat = stod(fields[7]);
                node.cellID = fields[4]; // Cell ID
                
                qt.insert(node);
                kdt.insert(node);
                count++;
            } catch (...) {
                continue; // Skip bad rows
            }
        }
    }
    cout << "Successfully loaded " << count << " towers into trees!\n";
}

int main() {
    // 1. Initialize Trees
    Boundary flBounds{ MIN_LON, MAX_LON, MIN_LAT, MAX_LAT };
    QuadTree quadTree(flBounds, 4);
    KDTree kdTree;

    // 2. Load Data
    cout << "Loading dataset... Please wait.\n";
    loadCSV("resources/data/processed/florida_cell_nodes.csv", quadTree, kdTree);

    // 3. Setup SFML Window
    const int WIDTH = 800;
    const int HEIGHT = 800;
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "SignalScope - Dead Zone Finder");

    // Load Map Image (You need to put a map of Florida in resources/images/)
    sf::Texture mapTexture;
    if (!mapTexture.loadFromFile("resources/images/florida_map.png")) {
        cout << "Could not load map image!\n";
    }
    sf::Sprite mapSprite(mapTexture);
    
    // Scale image to fit window
    mapSprite.setScale(
        (float)WIDTH / mapTexture.getSize().x, 
        (float)HEIGHT / mapTexture.getSize().y
    );

    // State variables for UI
    bool useKDTree = true; // True = KDTree, False = QuadTree
    bool hasDroppedPin = false;
    double clickLat = 0, clickLon = 0;
    CellNode* nearestNode = nullptr;
    double searchTimeUs = 0;
    double signalStrength = 0;

    // 4. Main Application Loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Keyboard Toggle for Algorithms (Fallback if ImGui is too hard to setup today)
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    useKDTree = !useKDTree;
                    cout << "Switched to: " << (useKDTree ? "KD-Tree" : "QuadTree") << endl;
                }
            }

            // Mouse Click Detection
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    // Convert Pixel (X,Y) to Coordinate (Lat, Lon)
                    int mouseX = event.mouseButton.x;
                    int mouseY = event.mouseButton.y;

                    clickLon = MIN_LON + ((double)mouseX / WIDTH) * (MAX_LON - MIN_LON);
                    clickLat = MAX_LAT - ((double)mouseY / HEIGHT) * (MAX_LAT - MIN_LAT); // Y is inverted

                    hasDroppedPin = true;

                    // Benchmarking Timer
                    auto start = chrono::high_resolution_clock::now();

                    if (useKDTree) {
                        nearestNode = kdTree.nearestNeighbor(clickLat, clickLon);
                    } else {
                        nearestNode = quadTree.nearestNeighbor(clickLat, clickLon);
                    }

                    auto end = chrono::high_resolution_clock::now();
                    searchTimeUs = chrono::duration_cast<chrono::microseconds>(end - start).count();

                    // Calculate Math using Student C's code
                    if (nearestNode != nullptr) {
                        double dist = MathUtils::haversineDistance(clickLat, clickLon, nearestNode->lat, nearestNode->lon);
                        signalStrength = MathUtils::estimateSignalStrength(dist);
                        
                        cout << "--- Search Results ---\n";
                        cout << "Time: " << searchTimeUs << " us\n";
                        cout << "Signal: " << signalStrength << " dBm\n";
                    }
                }
            }
        }

        // 5. Drawing Phase
        window.clear();
        window.draw(mapSprite); // Draw background map

        if (hasDroppedPin && nearestNode != nullptr) {
            // Draw User Pin
            sf::CircleShape userPin(5);
            userPin.setFillColor(sf::Color::Blue);
            userPin.setPosition(
                ((clickLon - MIN_LON) / (MAX_LON - MIN_LON)) * WIDTH - 5,
                ((MAX_LAT - clickLat) / (MAX_LAT - MIN_LAT)) * HEIGHT - 5
            );
            window.draw(userPin);

            // Draw Nearest Tower Pin
            sf::CircleShape towerPin(5);
            towerPin.setFillColor(sf::Color::Red);
            int tX = ((nearestNode->lon - MIN_LON) / (MAX_LON - MIN_LON)) * WIDTH;
            int tY = ((MAX_LAT - nearestNode->lat) / (MAX_LAT - MIN_LAT)) * HEIGHT;
            towerPin.setPosition(tX - 5, tY - 5);
            window.draw(towerPin);

            // Draw Line between them
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(userPin.getPosition().x + 5, userPin.getPosition().y + 5), sf::Color::Black),
                sf::Vertex(sf::Vector2f(towerPin.getPosition().x + 5, towerPin.getPosition().y + 5), sf::Color::Black)
            };
            window.draw(line, 2, sf::Lines);
        }

        window.display();
    }

    return 0;
}