#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "Aircraft.h"
#include <fstream>

struct HashNode {
    string key;
    Aircraft* value;
    HashNode* next;

    HashNode(string k, Aircraft* v) : key(k), value(v), next(nullptr) {}
};

class HashTable {
private:
    HashNode** table;
    int capacity;
    int size;

    int hashFunction(string key);

public:
    HashTable(int tableSize = 100);
    ~HashTable();

    void insert(string flightID, Aircraft* aircraft);
    Aircraft* search(string flightID);
    bool remove(string flightID);
    bool update(string flightID, Aircraft* newData);
    int getSize() const;
    void displayAll() const;

    // For file I/O
    void getAllAircraft(Aircraft*** aircraftArray, int& count);

    // Save/Load functionality
    bool saveToFile(const char* filename);
    bool loadFromFile(const char* filename);
};

#endif