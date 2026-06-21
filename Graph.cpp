#include "Graph.h"
#include <iostream>
#include <limits.h>

// ANSI Color Codes
#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"

Graph::Graph(int initialCapacity) : nodeCount(0), capacity(initialCapacity) {
    nodes = new Node[capacity];
}

Graph::~Graph() {
    for(int i = 0; i < nodeCount; i++) {
        Edge* current = nodes[i].edges;
        while(current != nullptr) {
            Edge* temp = current;
            current = current->next;
            delete temp;
        }
    }
    delete[] nodes;
}

void Graph::resize() {
    capacity *= 2;
    Node* newNodes = new Node[capacity];
    for(int i = 0; i < nodeCount; i++) {
        newNodes[i] = nodes[i];
    }
    delete[] nodes;
    nodes = newNodes;
}

int Graph::addNode(string name, int x, int y, bool isAirport) {
    if(nodeCount >= capacity) resize();

    nodes[nodeCount] = Node(name, nodeCount, x, y, isAirport);
    return nodeCount++;
}

void Graph::addEdge(int source, int dest, int weight) {
    if(source >= nodeCount || dest >= nodeCount) return;

    Edge* newEdge = new Edge(dest, weight);
    newEdge->next = nodes[source].edges;
    nodes[source].edges = newEdge;
}

bool Graph::isNodeOccupied(int nodeId) {
    if(nodeId < 0 || nodeId >= nodeCount) return false;
    return nodes[nodeId].occupied;
}

void Graph::setNodeOccupied(int nodeId, bool occupied) {
    if(nodeId >= 0 && nodeId < nodeCount) {
        nodes[nodeId].occupied = occupied;
    }
}

Node* Graph::getNode(int nodeId) {
    if(nodeId >= 0 && nodeId < nodeCount) {
        return &nodes[nodeId];
    }
    return nullptr;
}

int Graph::getNodeCount() const {
    return nodeCount;
}

void Graph::dijkstra(int startNode, int* distances, int* previous) {
    bool* visited = new bool[nodeCount];

    for(int i = 0; i < nodeCount; i++) {
        distances[i] = INT_MAX;
        previous[i] = -1;
        visited[i] = false;
    }

    distances[startNode] = 0;

    for(int count = 0; count < nodeCount - 1; count++) {
        int minDist = INT_MAX;
        int u = -1;

        for(int v = 0; v < nodeCount; v++) {
            if(!visited[v] && distances[v] < minDist) {
                minDist = distances[v];
                u = v;
            }
        }

        if(u == -1) break;
        visited[u] = true;

        Edge* edge = nodes[u].edges;
        while(edge != nullptr) {
            int v = edge->destination;
            if(!visited[v] && distances[u] != INT_MAX &&
                distances[u] + edge->weight < distances[v]) {
                distances[v] = distances[u] + edge->weight;
                previous[v] = u;
            }
            edge = edge->next;
        }
    }

    delete[] visited;
}

void Graph::printShortestPath(int startNode, int endNode) {
    int* distances = new int[nodeCount];
    int* previous = new int[nodeCount];

    dijkstra(startNode, distances, previous);

    if(distances[endNode] == INT_MAX) {
        cout << RED << "No path exists!" << RESET << endl;
        delete[] distances;
        delete[] previous;
        return;
    }

    cout << "\n" << CYAN << "Shortest Path " << RESET;
    cout << "(Distance: " << YELLOW << distances[endNode] << " km" << RESET << "):" << endl;

    int* path = new int[nodeCount];
    int pathLength = 0;
    int current = endNode;

    while(current != -1) {
        path[pathLength++] = current;
        current = previous[current];
    }

    for(int i = pathLength - 1; i >= 0; i--) {
        cout << GREEN << nodes[path[i]].name << RESET;
        if(i > 0) cout << BLUE << " -> " << RESET;
    }
    cout << endl;

    delete[] path;
    delete[] distances;
    delete[] previous;
}

int Graph::findNearestAirport(int currentNode) {
    int* distances = new int[nodeCount];
    int* previous = new int[nodeCount];

    dijkstra(currentNode, distances, previous);

    int nearestAirport = -1;
    int minDist = INT_MAX;

    for(int i = 0; i < nodeCount; i++) {
        if(nodes[i].isAirport && distances[i] < minDist) {
            minDist = distances[i];
            nearestAirport = i;
        }
    }

    delete[] distances;
    delete[] previous;
    return nearestAirport;
}

int* Graph::calculatePath(int startNode, int endNode, int& pathLength) {
    int* distances = new int[nodeCount];
    int* previous = new int[nodeCount];

    dijkstra(startNode, distances, previous);

    if(distances[endNode] == INT_MAX) {
        pathLength = 0;
        delete[] distances;
        delete[] previous;
        return nullptr;
    }

    // Count path length
    pathLength = 0;
    int current = endNode;
    while(current != -1) {
        pathLength++;
        current = previous[current];
    }

    // Build path array
    int* path = new int[pathLength];
    current = endNode;
    for(int i = pathLength - 1; i >= 0; i--) {
        path[i] = current;
        current = previous[current];
    }

    delete[] distances;
    delete[] previous;
    return path;
}

int Graph::findNodeByName(string name) {
    for(int i = 0; i < nodeCount; i++) {
        if(nodes[i].name == name) {
            return i;
        }
    }
    return -1;
}

void Graph::getAllAirports(int* airportIds, int& count) {
    count = 0;
    for(int i = 0; i < nodeCount; i++) {
        if(nodes[i].isAirport) {
            airportIds[count++] = i;
        }
    }
}
