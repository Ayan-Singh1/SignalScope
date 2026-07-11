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

const double MIN_LON = -90.9; // left (alabama)
const double MAX_LON = -78.2; // right (south carolina)
const double MIN_LAT = 24.0;  // bottom (florida)
const double MAX_LAT = 37.0;  // top (georgia)

const int MAP_SIZE = 820;
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
                node.radioType = fields[0]; // LTE, 5G, GSM, etc
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

void drawSignalBars(sf::RenderWindow& window, int x, int y, int bars) {
    for (int i = 0; i < 4; i++) {
        sf::RectangleShape bar(sf::Vector2f(8, 10 + (i * 6)));
        bar.setPosition(x + (i * 12), y + (18 - (i * 6)));
        if (i < bars) bar.setFillColor(sf::Color(50, 200, 100)); 
        else bar.setFillColor(sf::Color(100, 100, 100, 100));    
        window.draw(bar);
    }
}

int main() {
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
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SignalScope - Project 2");
    sf::Texture mapTex;
    mapTex.loadFromFile("resources/images/southeast_map.png");
    sf::Sprite mapSprite(mapTex);
    mapSprite.setScale((float)MAP_SIZE / mapTex.getSize().x, (float)MAP_SIZE / mapTex.getSize().y);
    sf::Font font;
    font.loadFromFile("resources/fonts/arial.ttf");
    sf::RectangleShape uiPanel(sf::Vector2f(390, 780));
    uiPanel.setPosition(840, 20);
    uiPanel.setFillColor(sf::Color(30, 30, 40, 240)); 
    uiPanel.setOutlineThickness(1);
    uiPanel.setOutlineColor(sf::Color(255, 255, 255, 40));
    sf::Text txtTitle("SIGNALSCOPE DATA", font, 16);
    txtTitle.setPosition(860, 40);
    txtTitle.setFillColor(sf::Color(180, 180, 200));
    txtTitle.setStyle(sf::Text::Bold);

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

    sf::Text txtDisplay("", font, 18);
    txtDisplay.setPosition(860, 220);
    txtDisplay.setLineSpacing(1.5f);

    sf::RectangleShape hoverBox(sf::Vector2f(260, 35));
    hoverBox.setPosition(20, WINDOW_HEIGHT - 55);
    hoverBox.setFillColor(sf::Color(20, 20, 30, 220));
    sf::Text txtHover("X: --.---, Y: --.---", font, 16);
    txtHover.setPosition(35, WINDOW_HEIGHT - 47);

    // state vars 
    bool useKDTree = true;
    bool hasDroppedPin = false;
    double clickLat = 0, clickLon = 0;
    CellNode* nearestNode = nullptr;
    double searchTimeUs = 0;
    double signalStrength = 0;
    double distKm = 0;
    string signalStatus = "--";
    int numSignalBars = 0;
    double maxRangeKm = 40.0; // only default for LTE (adjusted based on radio type)

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::MouseMoved) {
                if (event.mouseMove.x <= MAP_SIZE) {
                    double hLon = MIN_LON + ((double)event.mouseMove.x / MAP_SIZE) * (MAX_LON - MIN_LON);
                    double hLat = MAX_LAT - ((double)event.mouseMove.y / MAP_SIZE) * (MAX_LAT - MIN_LAT);
                    stringstream ssHover;
                    ssHover << fixed << setprecision(3) << "Lon: " << hLon << " | Lat: " << hLat;
                    txtHover.setString(ssHover.str());
                }
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                int mX = event.mouseButton.x;
                int mY = event.mouseButton.y;
                if (mX >= 840) {
                    if (mY >= 80 && mY <= 125) {
                        useKDTree = !useKDTree;
                        txtBtnAlgo.setString(useKDTree ? "Algorithm: KD-Tree" : "Algorithm: QuadTree");
                    } else if (mY >= 135 && mY <= 180) {
                        hasDroppedPin = false;
                        nearestNode = nullptr;
                    }
                } 
                // map clicked
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

                        // line of sight & signal Logic
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

        // drawing for SFML
        window.clear(sf::Color(15, 15, 20)); 
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
            // coverage circle
            double kmPerPixel = 1140.0 / MAP_SIZE; 
            float radiusPixels = maxRangeKm / kmPerPixel;

            sf::CircleShape coverageArea(radiusPixels);
            coverageArea.setOrigin(radiusPixels, radiusPixels);
            coverageArea.setPosition(tX, tY);
            if (distKm > maxRangeKm) {
                coverageArea.setFillColor(sf::Color(255, 50, 50, 40));
            } else {
                coverageArea.setFillColor(sf::Color(50, 255, 100, 40));
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

        if (hasDroppedPin && nearestNode) {
            drawSignalBars(window, 860, 565, numSignalBars);
        }

        window.draw(hoverBox);
        window.draw(txtHover);
        window.display();
    }
    return 0;
}