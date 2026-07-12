# SignalScope

**SignalScope** is an interactive C++ application designed to visualize and analyze cell node coverage across 4 states within the Southeastern United States (Florida, Georgia, Alabama, and South Carolina). 

SignalScope uses real-world tower data to allow users to click anywhere on the map to instantly identify the nearest cell node, calculate real-world signal degradation, and determine if a location is a "Dead Zone" based on tower coverage.

The project serves as a performance benchmark between two advanced spatial data structures: the **QuadTree** and the **2D K-D Tree**, both implemented from scratch in C++.

---

## Features
* **Two Custom Data Structure Algorithms:** Compares the microsecond search times of a QuadTree vs. a KD-Tree when querying a massive dataset (116k+ data points).
* **Real-World Engineering Math:** Calculates distances using the **Haversine Formula** (accounting for Earth's curvature) and estimates signal strength (dBm) using the **Free-Space Path Loss (FSPL)** model.
* **Dynamic Range & Line of Sight:** Evaluates network type (`LTE`, `GSM`, `5G`) to dynamically render accurate coverage radii. Automatically detects "Dead Zones" if a user is out of range.
* **Interactive Graphical UI:** Built purely in C++ using the SFML multimedia library.

---

## Dataset & Map Quirks
This project utilizes the [OpenCelliD](https://opencellid.org/) database, containing well over **116,000+** real-world cell nodes for the selected states. 

**A note on accuracy and edge-cases:**
1. **Maritime Nodes:** If you drop a pin in the middle of the Atlantic Ocean or Gulf of Mexico and it connects to a tower, this is *not a bug*. OpenCelliD tracks maritime cell nodes placed on buoys, oil rigs, and large cruise ships. 
2. **Map Projection Warping:** The Earth is a sphere, but our map is a flat 2D image. Because we use standard linear interpolation to map Longitude/Latitude to X/Y screen pixels, there is slight geographical warping near the northern and western borders. Pins may appear slightly offset from their exact real-world borders due to this projection constraint.

---

## Prerequisites & Dependencies
To compile and run this project, your system must have **CMake (3.10+)**, a C++17 compiler, and the **SFML 2.5+** library installed.

### Installing SFML
**macOS (Homebrew):**
```bash
brew install sfml
```

**Windows (MSYS2 / MinGW):**
```bash
pacman -S mingw-w64-x86_64-sfml
```

**Linux (Debian/Ubuntu):**
```bash
sudo apt-get install libsfml-dev
```

---

## Build & Run Instructions
Once SFML is installed on your machine, follow these steps to build the project using CMake:

1. **Clone the repository:**
   ```bash
   git clone <your-repo-link-here>
   cd SignalScope
   ```

2. **Create a build directory and navigate into it:**
   ```bash
   mkdir build
   cd build
   ```

3. **Generate the build files with CMake:**
   ```bash
   cmake ..
   ```

4. **Compile the project:**
   ```bash
   # On Mac/Linux:
   make

   # On Windows:
   cmake --build .
   ```

5. **Run the executable (ensure all .dll & resources folders are in the same location as the .exe):**
   ```bash
   # On Mac/Linux:
   ./SignalScope

   # On Windows:
   SignalScope.exe
   ```

---

## How to Use
1. **Launch the Executable:** Wait a few seconds for the terminal to parse and load the 116,000+ CSV rows into the tree structures.
2. **Scan for Coverage:** Click anywhere on the map to drop a pin. The program will draw a line to the nearest tower and display a coverage radius.
3. **Analyze Metrics:** Look at the right-hand panel to view the exact coordinate data, signal status (Excellent, Good, Weak, or DEAD ZONE), and the dBm power.
4. **Compare Algorithms:** Click the "Algorithm" button in the UI panel to toggle between the **KD-Tree** and the **QuadTree**. Drop another pin and observe the difference in `Search Time (us)` to see which data structure performs spatial queries faster!

---

## Collaborators
Project 2 - COP3530 Summer 2026.

* **Ayan Singh** - Repository & Environment Setup, UI/UX Integration w/trees, SFML Rendering, and Code Refactoring for compatability.
* **Mikelangelo Mutti** - Dataset Filtering, CSV Parsing, and QuadTree Implementation.
* **Hitesh Katikaneni** - RF Physics Math Engine (Haversine/FSPL) and KD-Tree Implementation.

---
