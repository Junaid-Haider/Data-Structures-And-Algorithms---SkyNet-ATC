#include "Radar.h"
#include <iostream>

// ANSI Color Codes
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

Radar::Radar(Graph* graph, HashTable* registry)
    : airspace(graph), aircraftRegistry(registry) {
    clearGrid();
}

void Radar::clearGrid() {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = '.';
        }
    }
}

void Radar::placeAirports() {
    for (int i = 0; i < airspace->getNodeCount(); i++) {
        Node* node = airspace->getNode(i);
        if (node != nullptr && node->isAirport) {
            int x = node->gridX;
            int y = node->gridY;
            if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
                grid[y][x] = 'A';
            }
        }
    }
}

void Radar::placeAircraft() {
    Aircraft** allAircraft;
    int count;
    aircraftRegistry->getAllAircraft(&allAircraft, count);

    for (int i = 0; i < count; i++) {
        if (allAircraft[i]->status == IN_FLIGHT ||
            allAircraft[i]->status == EMERGENCY) {
            int x = allAircraft[i]->gridX;
            int y = allAircraft[i]->gridY;

            if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
                if (grid[y][x] == '.') {
                    grid[y][x] = 'P';
                }
                else if (grid[y][x] == 'P') {
                    grid[y][x] = 'X'; // Collision warning
                }
            }
        }
    }

    delete[] allAircraft;
}

void Radar::refresh() {
    clearGrid();
    placeAirports();
    placeAircraft();
}

void Radar::display() {
    refresh();

    cout << "\n";
    cout << CYAN << "  =============================================" << RESET << endl;

    for (int i = 0; i < GRID_SIZE; i++) {
        cout << CYAN << "  |" << RESET;
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == 'A') {
                cout << GREEN << grid[i][j] << RESET << " ";
            }
            else if (grid[i][j] == 'P') {
                cout << YELLOW << grid[i][j] << RESET << " ";
            }
            else if (grid[i][j] == 'X') {
                cout << RED << BOLD << grid[i][j] << RESET << " ";
            }
            else {
                cout << BLUE << grid[i][j] << RESET << " ";
            }
        }
        cout << CYAN << "|" << RESET << endl;
    }

    cout << CYAN << "  =============================================" << RESET << endl;

    cout << "\n  Legend: "
        << GREEN << "[A]" << RESET << " Airport  "
        << YELLOW << "[P]" << RESET << " Plane  "
        << BLUE << "[.]" << RESET << " Empty Sky  "
        << RED << BOLD << "[X]" << RESET << " Collision Warning\n" << endl;
}

bool Radar::isValidPosition(int x, int y) {
    return (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE);
}

int Radar::getGridSize() {
    return GRID_SIZE;
}
