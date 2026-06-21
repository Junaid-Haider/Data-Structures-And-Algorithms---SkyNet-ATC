#include "HashTable.h"
#include <iostream>

// ANSI Color Codes
#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

HashTable::HashTable(int tableSize) : capacity(tableSize), size(0) {
    table = new HashNode * [capacity];
    for (int i = 0; i < capacity; i++) {
        table[i] = nullptr;
    }
}

HashTable::~HashTable() {
    for (int i = 0; i < capacity; i++) {
        HashNode* current = table[i];
        while (current != nullptr) {
            HashNode* temp = current;
            current = current->next;
            delete temp;
        }
    }
    delete[] table;
}

int HashTable::hashFunction(string key) {
    int sum = 0;
    for (int i = 0; i < key.length(); i++) {
        sum += (int)key[i];
    }
    return sum % capacity;
}

void HashTable::insert(string flightID, Aircraft* aircraft) {
    int index = hashFunction(flightID);

    HashNode* current = table[index];
    while (current != nullptr) {
        if (current->key == flightID) {
            current->value = aircraft;
            return;
        }
        current = current->next;
    }

    HashNode* newNode = new HashNode(flightID, aircraft);
    newNode->next = table[index];
    table[index] = newNode;
    size++;
}

Aircraft* HashTable::search(string flightID) {
    int index = hashFunction(flightID);

    HashNode* current = table[index];
    while (current != nullptr) {
        if (current->key == flightID) {
            return current->value;
        }
        current = current->next;
    }

    return nullptr;
}

bool HashTable::remove(string flightID) {
    int index = hashFunction(flightID);

    HashNode* current = table[index];
    HashNode* prev = nullptr;

    while (current != nullptr) {
        if (current->key == flightID) {
            if (prev == nullptr) {
                table[index] = current->next;
            }
            else {
                prev->next = current->next;
            }
            delete current;
            size--;
            return true;
        }
        prev = current;
        current = current->next;
    }

    return false;
}

bool HashTable::update(string flightID, Aircraft* newData) {
    Aircraft* aircraft = search(flightID);
    if (aircraft != nullptr) {
        *aircraft = *newData;
        return true;
    }
    return false;
}

int HashTable::getSize() const {
    return size;
}

void HashTable::displayAll() const {
    cout << "\n" << CYAN << "========== All Active Flights ==========" << RESET << endl;
    int count = 0;

    for (int i = 0; i < capacity; i++) {
        HashNode* current = table[i];
        while (current != nullptr) {
            count++;
            cout << YELLOW << count << ". " << RESET
                << BOLD << current->value->flightID << RESET
                << " - " << current->value->getStatusString()
                << " - Priority: " << current->value->getPriorityString() << endl;
            current = current->next;
        }
    }

    if (count == 0) {
        cout << YELLOW << "No active flights." << RESET << endl;
    }
    cout << CYAN << "========================================\n" << RESET << endl;
}

void HashTable::getAllAircraft(Aircraft*** aircraftArray, int& count) {
    count = size;
    *aircraftArray = new Aircraft * [count];

    int index = 0;
    for (int i = 0; i < capacity; i++) {
        HashNode* current = table[i];
        while (current != nullptr) {
            (*aircraftArray)[index++] = current->value;
            current = current->next;
        }
    }
}

bool HashTable::saveToFile(const char* filename) {
    ofstream outFile(filename);

    if (!outFile.is_open()) {
        return false;
    }

    // Write number of aircraft
    outFile << size << endl;

    // Write each aircraft data
    for (int i = 0; i < capacity; i++) {
        HashNode* current = table[i];
        while (current != nullptr) {
            Aircraft* aircraft = current->value;
            outFile << aircraft->flightID << endl;
            outFile << aircraft->model << endl;
            outFile << aircraft->fuelLevel << endl;
            outFile << aircraft->origin << endl;
            outFile << aircraft->destination << endl;
            outFile << (int)aircraft->status << endl;
            outFile << (int)aircraft->priority << endl;
            outFile << aircraft->gridX << endl;
            outFile << aircraft->gridY << endl;
            outFile << aircraft->currentNode << endl;
            outFile << aircraft->timestamp << endl;

            current = current->next;
        }
    }

    outFile.close();
    return true;
}

bool HashTable::loadFromFile(const char* filename) {
    ifstream inFile(filename);

    if (!inFile.is_open()) {
        return false;
    }

    int count;
    inFile >> count;
    inFile.ignore(); // Ignore newline

    for (int i = 0; i < count; i++) {
        string flightID, model, origin, destination;
        double fuelLevel;
        int status, priority, gridX, gridY, currentNode;
        long long timestamp;

        getline(inFile, flightID);
        getline(inFile, model);
        inFile >> fuelLevel;
        inFile.ignore();
        getline(inFile, origin);
        getline(inFile, destination);
        inFile >> status >> priority >> gridX >> gridY >> currentNode >> timestamp;
        inFile.ignore();

        Aircraft* aircraft = new Aircraft(flightID, model, fuelLevel, origin,
            destination, (Priority)priority,
            gridX, gridY, currentNode);
        aircraft->status = (Status)status;
        aircraft->timestamp = timestamp;

        insert(flightID, aircraft);
    }

    inFile.close();
    return true;
}