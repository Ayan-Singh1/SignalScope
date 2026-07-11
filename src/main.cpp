#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include <SFML/Graphics.hpp>
#include <iomanip>

#include "CellNode.h"
#include "QuadTree.h"
#include "KDTree.h"
#include "MathUtils.h"

using namespace std;

// --- CALIBRATION BOUNDS ---
// Tweak these if the pins don't perfectly align with the map image!
const double MIN_LON = -90.9; // Left edge of Alabama
const double MAX_LON = -78.2; // Right edge of South Carolina/Ocean
const double MIN_LAT = 24.0;  // Bottom edge (Below Key West)
const double MAX_LAT = 37.0;  // Top edge of GA/AL

const int MAP_SIZE = 820; // Locks the map into a perfect square
const int WINDOW_WIDTH = 1250;
const int WINDOW_HEIGHT = 820;

void loadCSV(const string& filename, QuadTree& qt, KDTree& kdt, int& totalCount) {
    ifstream file(filename);
    if (!file.is_open()) return;
    
    string line;
    getline(file, line); 
    while (getline(file, line)) {
        stringstream ss(line);
        string field;
        vector<string> fields;
        while (getline(ss, field, ',')) fields.push_back(field);
        
        if (fields.size() >= 8) {
            try {
                CellNode node;
                node.radioType = fields[0]; // e.g., LTE, GSM
                node.cellID = fields[4];
                node.lon = stod(fields[6]);
                node.lat = stod(fields[7]);
                qt.insert(node);
                kdt.insert(node);
                totalCount++;
            } catch (...) { continue; }
        }
    }
}

// Helper function to draw Cell Service Bars
void drawSignalBars(sf::RenderWindow& window, int x, int y, int bars) {
    for (int i = 0; i < 4; i++) {
        sf::RectangleShape bar(sf::Vector2f(8, 10 + (i * 6)));
        bar.setPosition(x + (i * 12), y + (18 - (i * 6)));
        if (i < bars) bar.setFillColor(sf::Color(50, 200, 100)); // Green filled bar
        else bar.setFillColor(sf::Color(100, 100, 100, 100));     // Empty grey bar
        window.draw(bar);
    }
}

int main() {
    // 1. Init Data
    Boundary bounds{ MIN_LON, MAX_LON, MIN_LAT, MAX_LAT };
    QuadTree quadTree(bounds, 4);
    KDTree kdTree;
    
    int totalCount = 0;
    cout << "Loading datasets... Please wait.\n";
    loadCSV("resources/data/processed/florida_cell_nodes.csv", quadTree, kdTree, totalCount);
    loadCSV("resources/data/processed/georgia_cell_nodes.csv", quadTree, kdTree, totalCount);
    loadCSV("resources/data/processed/alabama_cell_nodes.csv", quadTree, kdTree, totalCount);
    loadCSV("resources/data/processed/southcarolina_cell_nodes.csv", quadTree, kdTree, totalCount);
    cout << "Successfully loaded " << totalCount << " towers!\n";

    // 2. Setup SFML Window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SignalScope - Project 2");

    sf::Texture mapTex;
    mapTex.loadFromFile("resources/images/southeast_map.png");
    sf::Sprite mapSprite(mapTex);
    // Lock map strictly to MAP_SIZE x MAP_SIZE to prevent stretching
    mapSprite.setScale((float)MAP_SIZE / mapTex.getSize().x, (float)MAP_SIZE / mapTex.getSize().y);

    sf::Font font;
    font.loadFromFile("resources/fonts/arial.ttf");

    // 3. UI Panel (Floating Glass Design on the right)
    sf::RectangleShape uiPanel(sf::Vector2f(390, 780));
    uiPanel.setPosition(840, 20);
    uiPanel.setFillColor(sf::Color(30, 30, 40, 240)); 
    uiPanel.setOutlineThickness(1);
    uiPanel.setOutlineColor(sf::Color(255, 255, 255, 40));

    sf::Text txtTitle("SIGNALSCOPE DATA", font, 16);
    txtTitle.setPosition(860, 40);
    txtTitle.setFillColor(sf::Color(180, 180, 200));
    txtTitle.setStyle(sf::Text::Bold);

    // Buttons
    sf::RectangleShape btnAlgo(sf::Vector2f(350, 45));
    btnAlgo.setPosition(860, 80);
    btnAlgo.setFillColor(sf::Color(60, 60, 80));
    sf::Text txtBtnAlgo("Algorithm: KD-Tree", font, 18);
    txtBtnAlgo.setPosition(880, 92);

    sf::RectangleShape btnClear(sf::Vector2f(350, 45));
    btnClear.setPosition(860, 135);
    btnClear.setFillColor(sf::Color(140, 50, 60));
    sf::Text txtBtnClear("Clear Map Markers", font, 18);
    txtBtnClear.setPosition(880, 147);

    // Display Text
    sf::Text txtDisplay("", font, 18);
    txtDisplay.setPosition(860, 220);
    txtDisplay.setLineSpacing(1.5f);

    // Hover Box
    sf::RectangleShape hoverBox(sf::Vector2f(260, 35));
    hoverBox.setPosition(20, WINDOW_HEIGHT - 55);
    hoverBox.setFillColor(sf::Color(20, 20, 30, 220));
    sf::Text txtHover("X: --.---, Y: --.---", font, 16);
    txtHover.setPosition(35, WINDOW_HEIGHT - 47);

    // State Variables
    bool useKDTree = true;
    bool hasDroppedPin = false;
    double clickLat = 0, clickLon = 0;
    CellNode* nearestNode = nullptr;
    double searchTimeUs = 0;
    double signalStrength = 0;
    double distKm = 0;
    string signalStatus = "--";
    int numSignalBars = 0;
    double maxRangeKm = 40.0; // Default LTE range

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            // --- HOVER LOGIC (Only calculate if hovering over map) ---
            if (event.type == sf::Event::MouseMoved) {
                if (event.mouseMove.x <= MAP_SIZE) {
                    double hLon = MIN_LON + ((double)event.mouseMove.x / MAP_SIZE) * (MAX_LON - MIN_LON);
                    double hLat = MAX_LAT - ((double)event.mouseMove.y / MAP_SIZE) * (MAX_LAT - MIN_LAT);
                    stringstream ssHover;
                    ssHover << fixed << setprecision(3) << "Lon: " << hLon << " | Lat: " << hLat;
                    txtHover.setString(ssHover.str());
                }
            }

            // --- CLICK LOGIC ---
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                int mX = event.mouseButton.x;
                int mY = event.mouseButton.y;

                // UI Panel Clicked
                if (mX >= 840) {
                    if (mY >= 80 && mY <= 125) {
                        useKDTree = !useKDTree;
                        txtBtnAlgo.setString(useKDTree ? "Algorithm: KD-Tree" : "Algorithm: QuadTree");
                    } else if (mY >= 135 && mY <= 180) {
                        hasDroppedPin = false;
                        nearestNode = nullptr;
                    }
                } 
                // Map Clicked
                else if (mX <= MAP_SIZE) {
                    clickLon = MIN_LON + ((double)mX / MAP_SIZE) * (MAX_LON - MIN_LON);
                    clickLat = MAX_LAT - ((double)mY / MAP_SIZE) * (MAX_LAT - MIN_LAT);
                    hasDroppedPin = true;

                    auto start = chrono::high_resolution_clock::now();
                    if (useKDTree) nearestNode = kdTree.nearestNeighbor(clickLat, clickLon);
                    else nearestNode = quadTree.nearestNeighbor(clickLat, clickLon);
                    auto end = chrono::high_resolution_clock::now();
                    searchTimeUs = chrono::duration_cast<chrono::microseconds>(end - start).count();

                    if (nearestNode) {
                        distKm = MathUtils::haversineDistance(clickLat, clickLon, nearestNode->lat, nearestNode->lon);
                        signalStrength = MathUtils::estimateSignalStrength(distKm);

                        // Line of Sight & Signal Logic
                        // --- REALISTIC LOS & DYNAMIC RANGE LOGIC ---
                        maxRangeKm = 40.0;
                        if (nearestNode->radioType == "LTE") maxRangeKm = 35.0;
                        else if (nearestNode->radioType == "GSM") maxRangeKm = 65.0;
                        else if (nearestNode->radioType == "UMTS") maxRangeKm = 45.0; // 3G
                        else if (nearestNode->radioType == "NR" || nearestNode->radioType == "5G") maxRangeKm = 5.0; // 5G

                        if (distKm > maxRangeKm) { 
                            signalStatus = "DEAD ZONE (Out of Range)";
                            signalStrength = -120;
                            numSignalBars = 0;
                        } else if (signalStrength >= -75) { signalStatus = "Excellent"; numSignalBars = 4; }
                        else if (signalStrength >= -90) { signalStatus = "Good"; numSignalBars = 3; }
                        else if (signalStrength >= -100) { signalStatus = "Weak"; numSignalBars = 1; }
                        else { signalStatus = "DEAD ZONE"; numSignalBars = 0; }
                    }
                }
            }
        }

        // --- UPDATE DISPLAY TEXT ---
        if (hasDroppedPin && nearestNode) {
            stringstream ssDisplay;
            ssDisplay << fixed << setprecision(4)
                      << "PIN LOCATION\n"
                      << "Latitude: " << clickLat << "\n"
                      << "Longitude: " << clickLon << "\n\n"
                      << "NEAREST NODE DATA\n"
                      << "Tower ID: " << nearestNode->cellID << "\n"
                      << "Network: " << nearestNode->radioType << "\n"
                      << "Distance: " << setprecision(2) << distKm << " km\n\n"
                      << "ALGORITHM METRICS\n"
                      << "Search Time: " << (int)searchTimeUs << " ms\n\n"
                      << "CONNECTION STATUS\n"
                      << "Strength: " << signalStatus << "\n"
                      << "Power: " << (int)signalStrength << " dBm";
            txtDisplay.setString(ssDisplay.str());

            if (numSignalBars == 0) txtDisplay.setFillColor(sf::Color(255, 100, 100)); 
            else txtDisplay.setFillColor(sf::Color::White);
        } else {
            txtDisplay.setString("Waiting for user input...\n\nClick anywhere on the map to \ndrop a pin and scan for cell nodes.");
            txtDisplay.setFillColor(sf::Color(150, 150, 150));
        }

        // --- DRAWING ---
        window.clear(sf::Color(15, 15, 20)); // Ambient dark background for the app
        window.draw(mapSprite);

        if (hasDroppedPin && nearestNode) {
            int tX = ((nearestNode->lon - MIN_LON) / (MAX_LON - MIN_LON)) * MAP_SIZE;
            int tY = ((MAX_LAT - nearestNode->lat) / (MAX_LAT - MIN_LAT)) * MAP_SIZE;
            int uX = ((clickLon - MIN_LON) / (MAX_LON - MIN_LON)) * MAP_SIZE;
            int uY = ((MAX_LAT - clickLat) / (MAX_LAT - MIN_LAT)) * MAP_SIZE;

            sf::Vertex line[2] = {
                sf::Vertex(sf::Vector2f(static_cast<float>(uX), static_cast<float>(uY)), sf::Color(0, 0, 0, 180)),
                sf::Vertex(sf::Vector2f(static_cast<float>(tX), static_cast<float>(tY)), sf::Color(0, 0, 0, 180))
            };
            window.draw(line, 2, sf::Lines);

            sf::CircleShape uPin(5); uPin.setFillColor(sf::Color(50, 150, 255)); uPin.setPosition(uX - 5, uY - 5);
            sf::CircleShape tPin(5); tPin.setFillColor(sf::Color(255, 50, 100)); tPin.setPosition(tX - 5, tY - 5);
            uPin.setOutlineThickness(1); uPin.setOutlineColor(sf::Color::White);
            tPin.setOutlineThickness(1); tPin.setOutlineColor(sf::Color::White);
            // --- DRAW COVERAGE CIRCLE ---
            // Calculate approx pixels per kilometer (Width of map is ~1140km across these longitudes)
            double kmPerPixel = 1140.0 / MAP_SIZE; 
            float radiusPixels = maxRangeKm / kmPerPixel;

            sf::CircleShape coverageArea(radiusPixels);
            coverageArea.setOrigin(radiusPixels, radiusPixels); // Center the circle on the tower
            coverageArea.setPosition(tX, tY);

            // Make the circle Green if connected, Red if Dead Zone
            if (distKm > maxRangeKm) {
                coverageArea.setFillColor(sf::Color(255, 50, 50, 40)); // Transparent Red
            } else {
                coverageArea.setFillColor(sf::Color(50, 255, 100, 40)); // Transparent Green
            }

            window.draw(coverageArea);
            window.draw(uPin);
            window.draw(tPin);
        }

        window.draw(uiPanel);
        window.draw(txtTitle);
        window.draw(btnAlgo); window.draw(txtBtnAlgo);
        window.draw(btnClear); window.draw(txtBtnClear);
        window.draw(txtDisplay);
        
        // Draw Signal Bars next to text if a pin is dropped
        if (hasDroppedPin && nearestNode) {
            drawSignalBars(window, 860, 565, numSignalBars);
        }

        window.draw(hoverBox);
        window.draw(txtHover);
        window.display();
    }
    return 0;
}