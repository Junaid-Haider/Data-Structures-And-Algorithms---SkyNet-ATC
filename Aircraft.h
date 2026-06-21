#ifndef AIRCRAFT_H
#define AIRCRAFT_H

#include <string>
using namespace std;

enum Priority {
    CRITICAL = 1,    // Medical Emergency, Engine Failure, Hijack
    HIGH = 2,        // Low Fuel (< 10%)
    MEDIUM = 3,      // Commercial Passenger Flight
    LOW = 4          // Cargo / Private Jets
};

enum Status {
    IN_FLIGHT,
    LANDING,
    LANDED,
    EMERGENCY,
    CRASHED
};

class Aircraft {
public:
    string flightID;
    string model;
    double fuelLevel;
    string origin;
    string destination;
    Status status;
    Priority priority;
    int gridX;
    int gridY;
    int currentNode;
    int targetNode;
    long long timestamp;

    // Path information
    int* flightPath;
    int pathLength;
    int currentPathIndex;
    bool isMoving;

    Aircraft();
    Aircraft(string id, string mdl, double fuel, string org, string dest,
             Priority pri, int x, int y, int node);
    ~Aircraft();

    string getPriorityString() const;
    string getStatusString() const;
    void displayInfo() const;
    void setFlightPath(int* path, int length);
    bool moveToNextWaypoint();
};

#endif
