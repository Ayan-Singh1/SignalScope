#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

// est of Florida bounding box
const double FL_MIN_LAT = 24.0;
const double FL_MAX_LAT = 31.1;
const double FL_MIN_LON = -87.8;
const double FL_MAX_LON = -79.8;

// est of Georgia bounding box
const double GA_MIN_LAT = 30.3;
const double GA_MAX_LAT = 35.1;
const double GA_MIN_LON = -85.7;
const double GA_MAX_LON = -80.8;

// est of Alabama bounding box
const double AL_MIN_LAT = 30.1;
const double AL_MAX_LAT = 35.1;
const double AL_MIN_LON = -88.5;
const double AL_MAX_LON = -84.9;

// est of South Carolina bounding box
const double SC_MIN_LAT = 32.0;
const double SC_MAX_LAT = 35.3;
const double SC_MIN_LON = -83.4;
const double SC_MAX_LON = -78.5;

vector<string> splitCSVLine(const string& line) {
    vector<string> fields;
    string field;
    stringstream ss(line);

    while(getline(ss, field, ',')) {
        fields.push_back(field);
    }
    return fields;
}

bool isInFlorida(double lat, double lon) {
    return (lat >= FL_MIN_LAT && lat <= FL_MAX_LAT && lon >= FL_MIN_LON && lon <= FL_MAX_LON);
}
bool isInGeorgia(double lat, double lon) {
    return (lat >= GA_MIN_LAT && lat <= GA_MAX_LAT && lon >= GA_MIN_LON && lon <= GA_MAX_LON);
}

bool isInAlabama(double lat, double lon) {
    return (lat >= AL_MIN_LAT && lat <= AL_MAX_LAT && lon >= AL_MIN_LON && lon <= AL_MAX_LON);
}

bool isInSouthCarolina(double lat, double lon) {
    return (lat >= SC_MIN_LAT && lat <= SC_MAX_LAT && lon >= SC_MIN_LON && lon <= SC_MAX_LON);
}

void filterFile(const string& inputPath, ofstream& floridaFile, ofstream& georgiaFile, ofstream& alabamaFile, ofstream& southCarolinaFile) {
    ifstream inputFile(inputPath);

    if(!inputFile.is_open()) { 
        std::cout << "Could not open: " << inputPath << std::endl;
        return;
    }

    string line;
    int totalRows = 0;

    int floridaRows = 0;
    int georgiaRows = 0;
    int alabamaRows = 0;
    int southCarolinaRows = 0;

    while(getline(inputFile, line)) { 
        vector<string> fields = splitCSVLine(line);

        if(fields.size() < 8) {
            continue;
        }

        try {
            double lon = stod(fields[6]); 
            double lat = stod(fields[7]); 
            totalRows++;

            if(isInFlorida(lat, lon)) {
                floridaFile << line << '\n';
                floridaRows++;
             }

             if(isInGeorgia(lat, lon)) {
                georgiaFile << line << '\n';
                georgiaRows++;
             }

             if(isInAlabama(lat, lon)) {
                alabamaFile << line << '\n'; 
                alabamaRows++;
             }

             if(isInSouthCarolina(lat, lon)) {
                southCarolinaFile << line << '\n'; 
                southCarolinaRows++;
             }

        } catch(...) {
            continue;
        }
    }
    inputFile.close();

    std::cout << inputPath << ": kept"
     << floridaRows << " Florida rows out of "
     << totalRows << " usable rows" << std::endl;

     std::cout << inputPath << ": kept"
     << georgiaRows << " Georgia rows out of "
     << totalRows << " usable rows" << std::endl;

     std::cout << inputPath << ": kept"
     << alabamaRows << " Alabama rows out of "
     << totalRows << " usable rows" << std::endl;

     std::cout << inputPath << ": kept"
     << southCarolinaRows << " South Carolina rows out of "
     << totalRows << " usable rows" << std::endl;
}

int main() {
    vector<string> inputFiles = {
        "resources/data/raw/310.csv",
        "resources/data/raw/311.csv",
        "resources/data/raw/312.csv",
        "resources/data/raw/313.csv",
        "resources/data/raw/314.csv",
    };

    ofstream floridaFile("resources/data/processed/florida_cell_nodes.csv"); 
    ofstream georgiaFile("resources/data/processed/georgia_cell_nodes.csv"); 
    ofstream alabamaFile("resources/data/processed/alabama_cell_nodes.csv"); 
    ofstream southCarolinaFile("resources/data/processed/southcarolina_cell_nodes.csv");


    if(!floridaFile.is_open()) {
        std::cout << "Could not create florida_cell_nodes.csv" << std::endl;
        return 1;
    }
    if(!georgiaFile.is_open()) {
        std::cout << "Could not create georgia_cell_nodes.csv" << std::endl;
        return 1;
    }
    if(!alabamaFile.is_open()) {
        std::cout << "Could not create alabama_cell_nodes.csv" << std::endl;
        return 1;
    }
    if(!southCarolinaFile.is_open()) {
        std::cout << "Could not create southcarolina_cell_nodes.csv" << std::endl;
        return 1;
    }

    string header = "radio,mcc,net,area,cell,unit,lon,lat,range,samples,changeable,created,updated,averageSignal\n"; 

    floridaFile << header;
    georgiaFile << header;
    alabamaFile << header;
    southCarolinaFile << header;

    for(const string& inputPath : inputFiles) {
        filterFile(inputPath, floridaFile, georgiaFile, alabamaFile, southCarolinaFile);
    }
    floridaFile.close();
    georgiaFile.close();
    alabamaFile.close();
    southCarolinaFile.close();

    std::cout << "Completed. Created resources/data/processed/florida_cell_nodes.csv" << std::endl;
    std::cout << "Completed. Created resources/data/processed/georgia_cell_nodes.csv" << std::endl;
    std::cout << "Completed. Created resources/data/processed/alabama_cell_nodes.csv" << std::endl;
    std::cout << "Completed. Created resources/data/processed/southcarolina_cell_nodes.csv" << std::endl;

    return 0;
}