#include "Aircraft.h"
#include <iostream>
#include <ctime>

// ANSI Color Codes
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

Aircraft::Aircraft() : flightID(""), model(""), fuelLevel(100.0),
    origin(""), destination(""), status(IN_FLIGHT),
    priority(MEDIUM), gridX(0), gridY(0),
    currentNode(0), targetNode(-1), timestamp(0),
    flightPath(nullptr), pathLength(0),
    currentPathIndex(0), isMoving(false) {}

Aircraft::Aircraft(string id, string mdl, double fuel, string org,
                   string dest, Priority pri, int x, int y, int node)
    : flightID(id), model(mdl), fuelLevel(fuel), origin(org),
    destination(dest), status(IN_FLIGHT), priority(pri),
    gridX(x), gridY(y), currentNode(node), targetNode(-1),
    flightPath(nullptr), pathLength(0), currentPathIndex(0),
    isMoving(false) {
    timestamp = time(0);
}

Aircraft::~Aircraft() {
    if(flightPath != nullptr) {
        delete[] flightPath;
    }
}

string Aircraft::getPriorityString() const {
    switch(priority) {
    case CRITICAL: return string("CRITICAL");
    case HIGH: return  string("HIGH");
    case MEDIUM: return string("MEDIUM");
    case LOW: return  string("LOW");
    default: return "UNKNOWN";
    }
}

string Aircraft::getStatusString() const {
    switch(status) {
    case IN_FLIGHT: return "IN_FLIGHT";
    case LANDING: return "LANDING";
    case LANDED: return "LANDED";
    case EMERGENCY: return "EMERGENCY";
    case CRASHED: return "CRASHED";
    default: return "UNKNOWN";
    }
}

void Aircraft::displayInfo() const {
    cout << "\n" << CYAN << "========== Aircraft Information ==========" << RESET << endl;
    cout << BOLD << "Flight ID: " << RESET << YELLOW << flightID << RESET << endl;
    cout << "Model: " << model << endl;

    // Fuel level with color based on amount
    cout << "Fuel Level: ";
    if(fuelLevel < 20) cout << RED;
    else if(fuelLevel < 50) cout << YELLOW;
    else cout << GREEN;
    cout << fuelLevel << "%" << RESET << endl;

    cout << "Origin: " << CYAN << origin << RESET << endl;
    cout << "Destination: " << MAGENTA << destination << RESET << endl;
    cout << "Status: " << getStatusString() << endl;
    cout << "Priority: " << getPriorityString() << endl;
    cout << "Grid Position: (" << gridX << ", " << gridY << ")" << endl;
    cout << "Current Node: " << currentNode << endl;
    if(isMoving && pathLength > 0) {
        cout << "Flight Progress: " << currentPathIndex << "/" << pathLength << " waypoints" << endl;
    }
    cout << CYAN << "==========================================\n" << RESET << endl;
}

void Aircraft::setFlightPath(int* path, int length) {
    if(flightPath != nullptr) {
        delete[] flightPath;
    }

    pathLength = length;
    flightPath = new int[length];
    for(int i = 0; i < length; i++) {
        flightPath[i] = path[i];
    }
    currentPathIndex = 0;
    isMoving = true;
}

bool Aircraft::moveToNextWaypoint() {
    if(!isMoving || flightPath == nullptr || currentPathIndex >= pathLength) {
        isMoving = false;
        return false;
    }

    currentNode = flightPath[currentPathIndex];
    currentPathIndex++;

    // Decrease fuel slightly with each move
    fuelLevel -= 0.5;
    if(fuelLevel < 0) fuelLevel = 0;

    if(currentPathIndex >= pathLength) {
        isMoving = false;
        return false;
    }

    return true;
}
