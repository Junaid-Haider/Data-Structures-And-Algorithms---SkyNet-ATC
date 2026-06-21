#include "MinHeap.h"
#include <iostream>

// ANSI Color Codes
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

MinHeap::MinHeap(int initialCapacity) : capacity(initialCapacity), size(0) {
    heap = new Aircraft * [capacity];
}

MinHeap::~MinHeap() {
    delete[] heap;
}

int MinHeap::parent(int i) { return (i - 1) / 2; }
int MinHeap::leftChild(int i) { return 2 * i + 1; }
int MinHeap::rightChild(int i) { return 2 * i + 2; }

void MinHeap::swap(int i, int j) {
    Aircraft* temp = heap[i];
    heap[i] = heap[j];
    heap[j] = temp;
}

void MinHeap::resize() {
    capacity *= 2;
    Aircraft** newHeap = new Aircraft * [capacity];
    for (int i = 0; i < size; i++) {
        newHeap[i] = heap[i];
    }
    delete[] heap;
    heap = newHeap;
}

void MinHeap::heapifyUp(int index) {
    while (index > 0 && heap[parent(index)]->priority > heap[index]->priority) {
        swap(index, parent(index));
        index = parent(index);
    }
}

void MinHeap::heapifyDown(int index) {
    int smallest = index;
    int left = leftChild(index);
    int right = rightChild(index);

    if (left < size && heap[left]->priority < heap[smallest]->priority) {
        smallest = left;
    }

    if (right < size && heap[right]->priority < heap[smallest]->priority) {
        smallest = right;
    }

    if (smallest != index) {
        swap(index, smallest);
        heapifyDown(smallest);
    }
}

void MinHeap::insert(Aircraft* aircraft) {
    if (size >= capacity) resize();

    heap[size] = aircraft;
    heapifyUp(size);
    size++;
}

Aircraft* MinHeap::extractMin() {
    if (isEmpty()) return nullptr;

    Aircraft* minAircraft = heap[0];
    heap[0] = heap[size - 1];
    size--;
    heapifyDown(0);

    return minAircraft;
}

Aircraft* MinHeap::peek() const {
    if (isEmpty()) return nullptr;
    return heap[0];
}

bool MinHeap::isEmpty() const {
    return size == 0;
}

int MinHeap::getSize() const {
    return size;
}

bool MinHeap::updatePriority(string flightID, Priority newPriority) {
    for (int i = 0; i < size; i++) {
        if (heap[i]->flightID == flightID) {
            Priority oldPriority = heap[i]->priority;
            heap[i]->priority = newPriority;

            if (newPriority < oldPriority) {
                // Priority decreased (more urgent), bubble up
                heapifyUp(i);
            }
            else if (newPriority > oldPriority) {
                // Priority increased (less urgent), bubble down
                heapifyDown(i);
            }
            return true;
        }
    }
    return false;
}

void MinHeap::displayQueue() const {
    if (isEmpty()) {
        cout << "\n" << YELLOW << "Landing Queue is Empty\n" << RESET << endl;
        return;
    }

    cout << "\n" << CYAN << "========== Landing Queue ==========" << RESET << endl;
    cout << BOLD << "Position | Flight ID | Priority | Fuel" << RESET << endl;
    cout << "---------|-----------|-----------|---------" << endl;

    for (int i = 0; i < size && i < 10; i++) {
        cout << "   " << YELLOW << (i + 1) << RESET << "     | "
            << BOLD << heap[i]->flightID << RESET << "  | "
            << heap[i]->getPriorityString() << " | ";

        // Color code fuel level
        if (heap[i]->fuelLevel < 20) cout << RED;
        else if (heap[i]->fuelLevel < 50) cout << YELLOW;
        else cout << GREEN;
        cout << heap[i]->fuelLevel << "%" << RESET << endl;
    }

    if (size > 10) {
        cout << CYAN << "... and " << (size - 10) << " more aircraft" << RESET << endl;
    }
    cout << CYAN << "==================================\n" << RESET << endl;
}