#ifndef GRAPH_H
#define GRAPH_H

#include <string>
using namespace std;

struct Edge {
    int destination;
    int weight; //weighted graphs
    Edge* next;

    Edge(int dest, int w) : destination(dest), weight(w), next(nullptr) {}
};

struct Node {
    string name;
    int id;
    int gridX;
    int gridY;
    bool isAirport;
    Edge* edges;//line i.e path
    bool occupied;

    Node() : name(""), id(-1), gridX(0), gridY(0), isAirport(false),
        edges(nullptr), occupied(false) {}
    Node(string n, int i, int x, int y, bool airport)
        : name(n), id(i), gridX(x), gridY(y), isAirport(airport),
        edges(nullptr), occupied(false) {}
};

class Graph {
private:
    Node* nodes;
    int nodeCount;
    int capacity;

    void resize();

public:
    Graph(int initialCapacity = 50);
    ~Graph();

    int addNode(string name, int x, int y, bool isAirport);
    void addEdge(int source, int dest, int weight);
    bool isNodeOccupied(int nodeId);
    void setNodeOccupied(int nodeId, bool occupied);
    Node* getNode(int nodeId);
    int getNodeCount() const;

    // Dijkstra's Algorithm
    void dijkstra(int startNode, int* distances, int* previous);
    void printShortestPath(int startNode, int endNode);
    int findNearestAirport(int currentNode);

    // Path calculation
    int* calculatePath(int startNode, int endNode, int& pathLength);
    int findNodeByName(string name);
    void getAllAirports(int* airportIds, int& count);
};

#endif
