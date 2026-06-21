#include "AVLTree.h"
#include <iostream>
#include <fstream>
#include <ctime>

// ANSI Color Codes
#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"
#define BOLD    "\033[1m"

AVLTree::AVLTree() : root(nullptr) {}

AVLTree::~AVLTree() {
    destroy(root);
}

void AVLTree::destroy(AVLNode* node) {
    if (node != nullptr) {
        destroy(node->left);
        destroy(node->right);
        delete node;
    }
}

int AVLTree::height(AVLNode* node) {
    if (node == nullptr) return 0;
    return node->height;
}

int AVLTree::getBalance(AVLNode* node) {
    if (node == nullptr) return 0;
    return height(node->left) - height(node->right);
}

AVLNode* AVLTree::rightRotate(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = 1 + (height(y->left) > height(y->right) ? height(y->left) : height(y->right));
    x->height = 1 + (height(x->left) > height(x->right) ? height(x->left) : height(x->right));

    return x;
}

AVLNode* AVLTree::leftRotate(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = 1 + (height(x->left) > height(x->right) ? height(x->left) : height(x->right));
    y->height = 1 + (height(y->left) > height(y->right) ? height(y->left) : height(y->right));

    return y;
}

AVLNode* AVLTree::insert(AVLNode* node, LogRecord record) {
    if (node == nullptr) {
        return new AVLNode(record);
    }

    if (record.timestamp < node->data.timestamp) {
        node->left = insert(node->left, record);
    }
    else {
        node->right = insert(node->right, record);
    }

    node->height = 1 + (height(node->left) > height(node->right) ?
        height(node->left) : height(node->right));

    int balance = getBalance(node);

    // Left Left Case
    if (balance > 1 && record.timestamp < node->left->data.timestamp) {
        return rightRotate(node);
    }

    // Right Right Case
    if (balance < -1 && record.timestamp > node->right->data.timestamp) {
        return leftRotate(node);
    }

    // Left Right Case
    if (balance > 1 && record.timestamp > node->left->data.timestamp) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Right Left Case
    if (balance < -1 && record.timestamp < node->right->data.timestamp) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

void AVLTree::insertLog(Aircraft* aircraft) {
    LogRecord record(aircraft);
    root = insert(root, record);
}

void AVLTree::inOrderTraversal(AVLNode* node) const {
    if (node != nullptr) {
        inOrderTraversal(node->left);

        time_t t = node->data.timestamp;

        // Safe alternative to ctime - works on both Windows and Unix
#ifdef _WIN32
        char timeStr[26];
        ctime_s(timeStr, sizeof(timeStr), &t);
#else
        char* timeStr = ctime(&t);
#endif

        cout << "Flight: " << YELLOW << node->data.flightID << RESET
            << " | Model: " << node->data.model
            << " | Route: " << CYAN << node->data.origin << RESET
            << " -> " << MAGENTA << node->data.destination << RESET
            << " | Time: " << GREEN << timeStr << RESET;

        inOrderTraversal(node->right);
    }
}

void AVLTree::printLog() const {
    if (root == nullptr) {
        cout << "\n" << YELLOW << "No flight logs available.\n" << RESET << endl;
        return;
    }

    cout << "\n" << CYAN << BOLD << "========== Flight Log (Chronological) ==========" << RESET << endl;
    inOrderTraversal(root);
    cout << CYAN << "================================================\n" << RESET << endl;
}

int AVLTree::countNodes(AVLNode* node) {
    if (node == nullptr) return 0;
    return 1 + countNodes(node->left) + countNodes(node->right);
}

void AVLTree::saveInOrder(AVLNode* node, ofstream& outFile) {
    if (node != nullptr) {
        saveInOrder(node->left, outFile);

        outFile << node->data.flightID << endl;
        outFile << node->data.model << endl;
        outFile << node->data.origin << endl;
        outFile << node->data.destination << endl;
        outFile << node->data.timestamp << endl;
        outFile << (int)node->data.finalStatus << endl;

        saveInOrder(node->right, outFile);
    }
}

bool AVLTree::saveToFile(const char* filename) {
    ofstream outFile(filename);

    if (!outFile.is_open()) {
        return false;
    }

    // Write number of log entries
    int count = countNodes(root);
    outFile << count << endl;

    // Write all log entries
    saveInOrder(root, outFile);

    outFile.close();
    return true;
}

bool AVLTree::loadFromFile(const char* filename) {
    ifstream inFile(filename);

    if (!inFile.is_open()) {
        return false;
    }

    int count;
    inFile >> count;
    inFile.ignore(); // Ignore newline

    for (int i = 0; i < count; i++) {
        string flightID, model, origin, destination;
        long long timestamp;
        int status;

        getline(inFile, flightID);
        getline(inFile, model);
        getline(inFile, origin);
        getline(inFile, destination);
        inFile >> timestamp >> status;
        inFile.ignore();

        LogRecord record;
        record.flightID = flightID;
        record.model = model;
        record.origin = origin;
        record.destination = destination;
        record.timestamp = timestamp;
        record.finalStatus = (Status)status;

        root = insert(root, record);
    }

    inFile.close();
    return true;
}