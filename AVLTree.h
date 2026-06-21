#ifndef AVLTREE_H
#define AVLTREE_H

#include "Aircraft.h"
#include <fstream>

struct LogRecord {
    string flightID;
    string model;
    string origin;
    string destination;
    long long timestamp;
    Status finalStatus;

    LogRecord() : flightID(""), model(""), origin(""), destination(""),
        timestamp(0), finalStatus(LANDED) {
    }

    LogRecord(Aircraft* aircraft) {
        flightID = aircraft->flightID;
        model = aircraft->model;
        origin = aircraft->origin;
        destination = aircraft->destination;
        timestamp = aircraft->timestamp;
        finalStatus = aircraft->status;
    }
};

struct AVLNode {
    LogRecord data;
    AVLNode* left;
    AVLNode* right;
    int height;

    AVLNode(LogRecord record) : data(record), left(nullptr),
        right(nullptr), height(1) {
    }
};

class AVLTree {
private:
    AVLNode* root;

    int height(AVLNode* node);
    int getBalance(AVLNode* node);
    AVLNode* rightRotate(AVLNode* y);
    AVLNode* leftRotate(AVLNode* x);
    AVLNode* insert(AVLNode* node, LogRecord record);
    void inOrderTraversal(AVLNode* node) const;
    void destroy(AVLNode* node);

    // Helper functions for file I/O
    void saveInOrder(AVLNode* node, ofstream& outFile);
    int countNodes(AVLNode* node);

public:
    AVLTree();
    ~AVLTree();

    void insertLog(Aircraft* aircraft);
    void printLog() const;
    bool saveToFile(const char* filename);
    bool loadFromFile(const char* filename);
};

#endif