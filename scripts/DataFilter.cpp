#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

// Estimate of Florida bounding box
const double MIN_LAT = 24.0;
const double MAX_LAT = 31.1;
const double MIN_LON = -87.8;
const double MAX_LON = -79.8;

// Split CSV line by commas
vector<string> splitCSVLine(const string& line) {
    vector<string> fields;
    string field;
    stringstream ss(line);

    while(getline(ss, field, ',')) {
        fields.push_back(field);
    }
    return fields;
}

// Check whether a coordinate is inside FLorida's rough bounds
bool isInFlorida(double lat, double lon) {
    return (lat >= MIN_LAT && lat <= MAX_LAT && lon >= MIN_LON && lon <= MAX_LON);
}

// Read one CSV file and write Florida rows to the output
void filterFile(const string& inputPath, ofstream& outputFile) {
    ifstream inputFile(inputPath);

    if(!inputFile.is_open()) { // Check if file failed to open
        cout << "Could not open: " << inputPath << endl;
        return;
    }

    string line;
    int totalRows = 0;
    int floridaRows = 0;

    while(getline(inputFile, line)) { // Read one full CSV row and store it
        vector<string> fields = splitCSVLine(line); // Split row into individual columns

        if(fields.size() < 8) {
            continue;
        }

        try {
            double lon = stod(fields[6]); // OpenCelliD longitude col
            double lat = stod(fields[7]); // OpenCelliD latitude col
            totalRows++;

            if(isInFlorida(lat, lon)) {
                outputFile << line << '\n'; // Florida row written to output CSV
                floridaRows++;
             }
        } catch(...) {
            continue;
        }
    }
    inputFile.close();

    cout << inputPath << ": kept"
     << floridaRows << " Florida rows out of "
     << totalRows << " usable rows" << endl;
}

int main() {
    vector<string> inputFiles = {
        "resources/data/raw/310.csv",
        "resources/data/raw/311.csv",
        "resources/data/raw/312.csv",
        "resources/data/raw/313.csv",
        "resources/data/raw/314.csv",
    };

    ofstream outputFile("resources/data/processed/florida_cell_nodes.csv"); // Write cleaned Florida CSV file

    if(!outputFile.is_open()) {
        cout << "Could not create florida_cell_nodes.csv" << endl;
        return 1;
    }

    outputFile << "radio,mcc,net,area,cell,unit,lon,lat,range,samples,changeable,created,updated,averageSignal\n"; // Header row

    for(const string& inputPath : inputFiles) {
        filterFile(inputPath, outputFile); // Filter through each raw MCC CSV file and write Florida rows
    }
    outputFile.close();
    cout << "Completed. Created resources/data/processed/florida_cell_nodes.csv" << endl;

    return 0;
}