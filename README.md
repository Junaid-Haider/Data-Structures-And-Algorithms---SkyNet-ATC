# SkyNet ATC — Air Traffic Control Simulator

A real-time air traffic control simulation built with Qt, where every core system — aircraft lookup, landing prioritization, route-finding, and flight history — is powered by data structures implemented entirely from scratch rather than the standard library.

## What It Does

SkyNet ATC simulates a live airspace on a 25×25 radar grid. Flights are added with an origin, destination, and priority level, and the system calculates the shortest route between airports using a custom graph and Dijkstra's algorithm, then animates the aircraft along that path step by step. Landing order is managed by a min-heap, so high-priority aircraft (engine failure, medical emergencies, low fuel) jump the queue automatically, with support for declaring an in-flight emergency that re-prioritizes a flight on the fly. Every aircraft is tracked in a hash table for instant lookup by flight ID, and once a flight lands, it's logged into a self-balancing AVL tree that keeps the full flight history sorted and queryable. The whole system can be saved to and reloaded from disk.

## Features

- Real-time animated radar (25×25 grid) showing aircraft positions, airports, and live collision detection
- Custom hash table (separate chaining) for O(1) aircraft registry lookup, insert, update, and removal by flight ID
- Custom min-heap priority queue for landing order, supporting an emergency override (decrease-key) to re-prioritize a flight mid-simulation
- Custom AVL tree maintaining a sorted, self-balancing log of completed flights
- Custom graph (adjacency list) with Dijkstra's algorithm for shortest-path routing between airports
- Step-by-step aircraft movement along a calculated flight path, visualized live on the radar
- Save/load full system state (aircraft registry + flight log) to disk
- Dark-themed Qt Widgets interface with live tables for the landing queue and active aircraft

## Tech Stack

- **Language:** C++ (C++11)
- **Framework:** Qt 6 (Widgets module)
- **Build system:** qmake / Qt Creator

## Concepts Demonstrated

- Custom data structure implementations built without STL containers: hash table with chained collision resolution, binary min-heap with decrease-key, self-balancing AVL tree with full rotation logic (LL, RR, LR, RL), and an adjacency-list graph
- Dijkstra's shortest-path algorithm
- Hashing and collision handling
- Tree balancing and rotation in a self-balancing BST
- Priority queue design applied to a real scheduling problem
- File I/O for system state persistence
- Event-driven GUI programming with Qt signals and slots
- Custom `QWidget` subclassing with an overridden `paintEvent` for the radar rendering
- Separation of core logic (data structures) from the UI layer (`MainWindow`)

## How to Run

**Requirements:** Qt 6 (Widgets module), a C++11-compatible compiler (MinGW or MSVC on Windows, GCC/Clang on Linux/macOS), and either Qt Creator or `qmake` on the command line.

**Option 1 — Qt Creator (recommended):**

1. Open `SkyNetATC.pro` in Qt Creator
2. Select a kit (e.g. Desktop Qt 6.x MinGW 64-bit)
3. Build and Run (`Ctrl+R`)

**Option 2 — Command line:**

```bash
qmake SkyNetATC.pro
make
./SkyNetATC
```

(On Windows with MinGW, use `mingw32-make` instead of `make`, and run `SkyNetATC.exe`.)

## Collaborators

- 24F-3122 - Junaid Haider
- 24F-3097 - Muhammad Abdullah

## What We Learned

This project meant building real data structures end-to-end rather than relying on `std::unordered_map` or `std::priority_queue` — implementing collision resolution, heap rebalancing, and AVL rotations ourselves made the trade-offs each structure makes far more concrete than just using them. Wiring those structures into a live Qt GUI added another layer: keeping the radar, tables, and underlying data in sync on every update, and structuring the codebase so the simulation logic stayed cleanly separated from the UI instead of getting tangled into `mainwindow.cpp`.
