#ifndef MINHEAP_H
#define MINHEAP_H

#include "Aircraft.h"

class MinHeap {
private:
    Aircraft** heap;
    int capacity;
    int size;

    void heapifyUp(int index);
    void heapifyDown(int index);
    void swap(int i, int j);
    int parent(int i);
    int leftChild(int i);
    int rightChild(int i);
    void resize();

public:
    MinHeap(int initialCapacity = 50);
    ~MinHeap();

    void insert(Aircraft* aircraft);
    Aircraft* extractMin();
    Aircraft* peek() const;
    bool isEmpty() const;
    int getSize() const;

    // Emergency Override - Decrease Key
    bool updatePriority(string flightID, Priority newPriority);
    void displayQueue() const;
};

#endif