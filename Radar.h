#ifndef RADAR_H
#define RADAR_H

#include "Graph.h"
#include "HashTable.h"

class Radar {
private:
    static const int GRID_SIZE = 25;
    char grid[GRID_SIZE][GRID_SIZE];
    Graph* airspace;
    HashTable* aircraftRegistry;

    void clearGrid();
    void placeAirports();
    void placeAircraft();

public:
    Radar(Graph* graph, HashTable* registry);

    void display();
    void refresh();
    bool isValidPosition(int x, int y);
    static int getGridSize();
};

#endif