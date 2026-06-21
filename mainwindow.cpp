#include "mainwindow.h"
#include<iostream>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QInputDialog>
#include <QPainter>
#include <QHeaderView>
#include <QFileDialog>
#include <ctime>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("SkyNet Air Traffic Control System");
    setMinimumSize(1400, 900);

    initializeSystem();
    setupUI();

    // Auto-update timer
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &MainWindow::updateDisplay);
    updateTimer->start(1000); // Update every second
}

MainWindow::~MainWindow()
{
    delete airspace;
    delete landingQueue;
    delete aircraftRegistry;
    delete flightLog;
}

void MainWindow::initializeSystem()
{
    airspace = new Graph(50);
    landingQueue = new MinHeap(50);
    aircraftRegistry = new HashTable(100);
    flightLog = new AVLTree();

    // Create Airports
    int jfk = airspace->addNode("JFK", 2, 2, true);
    int lhr = airspace->addNode("LHR", 22, 2, true);
    int dxb = airspace->addNode("DXB", 2, 22, true);
    int hnd = airspace->addNode("HND", 22, 22, true);
    int cdg = airspace->addNode("CDG", 12, 12, true);

    // Create Waypoints
    int wp1 = airspace->addNode("WP1", 7, 7, false);
    int wp2 = airspace->addNode("WP2", 17, 7, false);
    int wp3 = airspace->addNode("WP3", 7, 17, false);
    int wp4 = airspace->addNode("WP4", 17, 17, false);

    // Add Edges
    airspace->addEdge(jfk, wp1, 150);
    airspace->addEdge(wp1, cdg, 200);
    airspace->addEdge(wp1, lhr, 300);
    airspace->addEdge(lhr, wp2, 150);
    airspace->addEdge(wp2, hnd, 400);
    airspace->addEdge(dxb, wp3, 180);
    airspace->addEdge(wp3, cdg, 220);
    airspace->addEdge(cdg, wp4, 150);
    airspace->addEdge(wp4, hnd, 250);
}

void MainWindow::setupUI()
{
    QWidget* centralWidget = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);

    // Left Panel - Radar
    QVBoxLayout* leftPanel = new QVBoxLayout();
    QGroupBox* radarGroup = new QGroupBox("Radar View");
    QVBoxLayout* radarLayout = new QVBoxLayout(radarGroup);

    radarWidget = new RadarWidget(this);
    radarWidget->setAirspace(airspace);
    radarWidget->setRegistry(aircraftRegistry);
    radarLayout->addWidget(radarWidget);

    QPushButton* refreshBtn = new QPushButton("Refresh Radar");
    connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::onRefreshRadar);
    radarLayout->addWidget(refreshBtn);

    leftPanel->addWidget(radarGroup);

    // Middle Panel - Controls
    QVBoxLayout* middlePanel = new QVBoxLayout();

    // Add Flight Section
    QGroupBox* addFlightGroup = new QGroupBox("Add New Flight");
    QVBoxLayout* addFlightLayout = new QVBoxLayout(addFlightGroup);

    QHBoxLayout* row1 = new QHBoxLayout();
    row1->addWidget(new QLabel("Flight ID:"));
    flightIDInput = new QLineEdit();
    flightIDInput->setPlaceholderText("e.g., PK-786");
    row1->addWidget(flightIDInput);

    row1->addWidget(new QLabel("Model:"));
    modelInput = new QLineEdit();
    modelInput->setPlaceholderText("e.g., Boeing-737");
    row1->addWidget(modelInput);
    addFlightLayout->addLayout(row1);

    QHBoxLayout* row2 = new QHBoxLayout();
    row2->addWidget(new QLabel("Fuel:"));
    fuelInput = new QSpinBox();
    fuelInput->setRange(0, 100);
    fuelInput->setValue(100);
    fuelInput->setSuffix("%");
    row2->addWidget(fuelInput);

    row2->addWidget(new QLabel("Priority:"));
    priorityCombo = new QComboBox();
    priorityCombo->addItem("CRITICAL", CRITICAL);
    priorityCombo->addItem("HIGH", HIGH);
    priorityCombo->addItem("MEDIUM", MEDIUM);
    priorityCombo->addItem("LOW", LOW);
    priorityCombo->setCurrentIndex(2);
    row2->addWidget(priorityCombo);
    addFlightLayout->addLayout(row2);

    QHBoxLayout* row3 = new QHBoxLayout();
    row3->addWidget(new QLabel("Origin Airport:"));
    originCombo = new QComboBox();
    row3->addWidget(originCombo);

    row3->addWidget(new QLabel("Destination:"));
    destCombo = new QComboBox();
    row3->addWidget(destCombo);
    addFlightLayout->addLayout(row3);

    QHBoxLayout* row4 = new QHBoxLayout();
    row4->addWidget(new QLabel("Grid X:"));
    gridXInput = new QSpinBox();
    gridXInput->setRange(0, 24);
    gridXInput->setValue(10);
    row4->addWidget(gridXInput);

    row4->addWidget(new QLabel("Grid Y:"));
    gridYInput = new QSpinBox();
    gridYInput->setRange(0, 24);
    gridYInput->setValue(10);
    row4->addWidget(gridYInput);
    addFlightLayout->addLayout(row4);

    QPushButton* addBtn = new QPushButton("Add Flight");
    addBtn->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 8px;");
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddFlight);
    addFlightLayout->addWidget(addBtn);

    // Populate airport dropdowns
    populateAirportCombos();

    middlePanel->addWidget(addFlightGroup);

    // NEW: Movement Control Section
    QGroupBox* movementGroup = new QGroupBox("Aircraft Movement");
    QVBoxLayout* movementLayout = new QVBoxLayout(movementGroup);

    QHBoxLayout* moveRow = new QHBoxLayout();
    moveRow->addWidget(new QLabel("Flight ID:"));
    moveFlightIDInput = new QLineEdit();
    moveFlightIDInput->setPlaceholderText("e.g., PK-786");
    moveRow->addWidget(moveFlightIDInput);
    movementLayout->addLayout(moveRow);

    moveStepBtn = new QPushButton("✈️ Move One Step");
    moveStepBtn->setStyleSheet("background-color: #FF9800; color: white; font-weight: bold; padding: 8px;");
    connect(moveStepBtn, &QPushButton::clicked, this, &MainWindow::onMoveAircraftStep);
    movementLayout->addWidget(moveStepBtn);

    middlePanel->addWidget(movementGroup);

    // Action Buttons
    QGroupBox* actionsGroup = new QGroupBox("Actions");
    QVBoxLayout* actionsLayout = new QVBoxLayout(actionsGroup);

    QPushButton* emergencyBtn = new QPushButton("🚨 Declare Emergency");
    emergencyBtn->setStyleSheet("background-color: #f44336; color: white; font-weight: bold;");
    connect(emergencyBtn, &QPushButton::clicked, this, &MainWindow::onDeclareEmergency);
    actionsLayout->addWidget(emergencyBtn);

    QPushButton* landBtn = new QPushButton("✈️ Land Aircraft");
    landBtn->setStyleSheet("background-color: #2196F3; color: white; font-weight: bold;");
    connect(landBtn, &QPushButton::clicked, this, &MainWindow::onLandAircraft);
    actionsLayout->addWidget(landBtn);

    QPushButton* searchBtn = new QPushButton("🔍 Search Flight");
    connect(searchBtn, &QPushButton::clicked, this, &MainWindow::onSearchFlight);
    actionsLayout->addWidget(searchBtn);

    QPushButton* routeBtn = new QPushButton("🗺️ Find Safe Route");
    connect(routeBtn, &QPushButton::clicked, this, &MainWindow::onFindRoute);
    actionsLayout->addWidget(routeBtn);

    QPushButton* logBtn = new QPushButton("📋 Show Flight Log");
    connect(logBtn, &QPushButton::clicked, this, &MainWindow::onShowFlightLog);
    actionsLayout->addWidget(logBtn);

    QHBoxLayout* saveLoadLayout = new QHBoxLayout();
    QPushButton* saveBtn = new QPushButton("💾 Save");
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::onSaveSystem);
    saveLoadLayout->addWidget(saveBtn);

    QPushButton* loadBtn = new QPushButton("📂 Load");
    connect(loadBtn, &QPushButton::clicked, this, &MainWindow::onLoadSystem);
    saveLoadLayout->addWidget(loadBtn);
    actionsLayout->addLayout(saveLoadLayout);

    middlePanel->addWidget(actionsGroup);

    // Log Output
    QGroupBox* logGroup = new QGroupBox("System Log");
    QVBoxLayout* logLayout = new QVBoxLayout(logGroup);
    logOutput = new QTextEdit();
    logOutput->setReadOnly(true);
    logLayout->addWidget(logOutput);
    middlePanel->addWidget(logGroup);

    // Right Panel - Tables
    QVBoxLayout* rightPanel = new QVBoxLayout();

    // Landing Queue Table
    QGroupBox* queueGroup = new QGroupBox("Landing Queue");
    QVBoxLayout* queueLayout = new QVBoxLayout(queueGroup);
    queueTable = new QTableWidget(0, 4);
    queueTable->setHorizontalHeaderLabels({"Position", "Flight ID", "Priority", "Fuel"});
    queueTable->horizontalHeader()->setStretchLastSection(true);
    queueLayout->addWidget(queueTable);
    rightPanel->addWidget(queueGroup);

    // Active Aircraft Table
    QGroupBox* aircraftGroup = new QGroupBox("Active Aircraft");
    QVBoxLayout* aircraftLayout = new QVBoxLayout(aircraftGroup);
    aircraftTable = new QTableWidget(0, 3);
    aircraftTable->setHorizontalHeaderLabels({"Flight ID", "Status", "Priority"});
    aircraftTable->horizontalHeader()->setStretchLastSection(true);
    aircraftLayout->addWidget(aircraftTable);
    rightPanel->addWidget(aircraftGroup);

    // Add panels to main layout
    mainLayout->addLayout(leftPanel, 2);
    mainLayout->addLayout(middlePanel, 2);
    mainLayout->addLayout(rightPanel, 1);

    setCentralWidget(centralWidget);

    logMessage("✓ System initialized successfully!", "green");
}

void MainWindow::populateAirportCombos()
{
    int airportIds[20];
    int count;
    airspace->getAllAirports(airportIds, count);

    originCombo->clear();
    destCombo->clear();

    for(int i = 0; i < count; i++) {
        Node* node = airspace->getNode(airportIds[i]);
        if(node) {
            QString airportName = QString::fromStdString(node->name);
            originCombo->addItem(airportName, airportIds[i]);
            destCombo->addItem(airportName, airportIds[i]);
        }
    }
}

void MainWindow::onAddFlight()
{
    QString flightID = flightIDInput->text().trimmed();
    QString model = modelInput->text().trimmed();
    int fuel = fuelInput->value();
    QString origin = originCombo->currentText();
    QString dest = destCombo->currentText();
    Priority priority = (Priority)priorityCombo->currentData().toInt();
    int x = gridXInput->value();
    int y = gridYInput->value();

    if(flightID.isEmpty() || model.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Please fill Flight ID and Model!");
        return;
    }

    if(origin == dest) {
        QMessageBox::warning(this, "Invalid Route", "Origin and Destination cannot be the same!");
        return;
    }

    int originNode = originCombo->currentData().toInt();
    int destNode = destCombo->currentData().toInt();

    // Calculate flight path
    int pathLength = 0;
    int* path = airspace->calculatePath(originNode, destNode, pathLength);

    if(path == nullptr || pathLength == 0) {
        QMessageBox::warning(this, "No Route", "No route available between selected airports!");
        return;
    }

    Aircraft* aircraft = new Aircraft(
        flightID.toStdString(),
        model.toStdString(),
        fuel,
        origin.toStdString(),
        dest.toStdString(),
        priority,
        x, y, originNode
        );

    // Set the flight path
    aircraft->setFlightPath(path, pathLength);
    aircraft->targetNode = destNode;

    // Show the planned route on radar in GREEN
    radarWidget->setPlannedPath(path, pathLength, flightID.toStdString());
    radarWidget->refresh();

    aircraftRegistry->insert(flightID.toStdString(), aircraft);
    landingQueue->insert(aircraft);

    logMessage("✓ Flight " + flightID + " added successfully!", "green");
    logMessage("  Route: " + origin + " → " + dest + " (" +
                   QString::number(pathLength) + " waypoints)", "blue");
    logMessage("  Green path displayed on radar", "green");

    delete[] path;

    // Clear inputs
    flightIDInput->clear();
    modelInput->clear();
    fuelInput->setValue(100);
    priorityCombo->setCurrentIndex(2);
    gridXInput->setValue(10);
    gridYInput->setValue(10);

    updateDisplay();
}

// NEW: Step-by-step movement function
void MainWindow::onMoveAircraftStep()
{
    QString flightID = moveFlightIDInput->text().trimmed();

    if(flightID.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a Flight ID!");
        return;
    }

    Aircraft* aircraft = aircraftRegistry->search(flightID.toStdString());

    if(aircraft == nullptr) {
        QMessageBox::warning(this, "Not Found", "Flight " + flightID + " not found!");
        return;
    }

    if(aircraft->status == LANDED) {
        QMessageBox::information(this, "Already Landed", "Flight " + flightID + " has already landed!");
        return;
    }

    if(!aircraft->isMoving || aircraft->flightPath == nullptr) {
        QMessageBox::warning(this, "No Path", "Flight " + flightID + " has no active flight path!");
        return;
    }

    // Move to next waypoint
    bool moved = aircraft->moveToNextWaypoint();

    if(moved) {
        // Update grid position to match current node
        Node* currentNode = airspace->getNode(aircraft->currentNode);
        if(currentNode) {
            aircraft->gridX = currentNode->gridX;
            aircraft->gridY = currentNode->gridY;
        }

        logMessage("✈️ Flight " + flightID + " moved to waypoint " +
                       QString::number(aircraft->currentPathIndex) + "/" +
                       QString::number(aircraft->pathLength), "blue");
        logMessage("  Fuel remaining: " + QString::number(aircraft->fuelLevel, 'f', 1) + "%",
                   aircraft->fuelLevel < 20 ? "red" : (aircraft->fuelLevel < 50 ? "orange" : "green"));

        // Check if fuel is critically low
        if(aircraft->fuelLevel < 10 && aircraft->priority != CRITICAL) {
            aircraft->priority = HIGH;
            landingQueue->updatePriority(flightID.toStdString(), HIGH);
            logMessage("⚠️ Low fuel warning! Priority elevated to HIGH", "orange");
        }
    } else {
        // Reached destination
        logMessage("✓ Flight " + flightID + " reached destination!", "green");
        logMessage("  Ready for landing. Use 'Land Aircraft' to complete.", "cyan");
        aircraft->status = LANDING;

        // Clear the green path for this flight
        radarWidget->clearPathForFlight(flightID.toStdString());
    }

    updateDisplay();
}

void MainWindow::onDeclareEmergency()
{
    QString flightID = QInputDialog::getText(this, "Declare Emergency", "Enter Flight ID:");

    if(flightID.isEmpty()) return;

    Aircraft* aircraft = aircraftRegistry->search(flightID.toStdString());

    if(aircraft == nullptr) {
        QMessageBox::warning(this, "Not Found", "Flight " + flightID + " not found!");
        return;
    }

    cout << "\nBefore Emergency:" << endl;
    cout << "Priority: " << aircraft->getPriorityString() << endl;
    cout << "Status: " << aircraft->getStatusString() << endl;

    // Update the aircraft object
    aircraft->status = EMERGENCY;
    Priority oldPriority = aircraft->priority;
    aircraft->priority = CRITICAL;

    // Update the heap
    bool heapUpdated = landingQueue->updatePriority(flightID.toStdString(), CRITICAL);

    if(heapUpdated) {
        cout << "\nAfter Emergency:" << endl;
        cout << "Priority: " << aircraft->getPriorityString() << endl;
        cout << "Status: " << aircraft->getStatusString() << endl;

        logMessage("🚨 EMERGENCY declared for " + flightID + "! Priority elevated to CRITICAL!", "red");
        updateDisplay();
    } else {
        logMessage("✗ Failed to update priority in landing queue!", "red");
        logMessage("Note: Aircraft may not be in landing queue yet.", "orange");
    }
}

void MainWindow::onLandAircraft()
{
    Aircraft* landing = landingQueue->extractMin();

    if(landing == nullptr) {
        QMessageBox::information(this, "Empty Queue", "No aircraft in landing queue!");
        return;
    }

    QString flightID = QString::fromStdString(landing->flightID);

    landing->status = LANDED;
    landing->timestamp = time(0);

    flightLog->insertLog(landing);
    aircraftRegistry->remove(landing->flightID);

    // IMPORTANT: Clear the green path when aircraft lands
    radarWidget->clearPathForFlight(landing->flightID);

    logMessage("✈️ Flight " + flightID + " landed successfully!", "blue");
    logMessage("  Green path removed from radar", "gray");
    updateDisplay();
}

void MainWindow::onSearchFlight()
{
    QString flightID = QInputDialog::getText(this, "Search Flight", "Enter Flight ID:");

    if(flightID.isEmpty()) return;

    Aircraft* aircraft = aircraftRegistry->search(flightID.toStdString());

    if(aircraft != nullptr) {
        QString info = QString(
                           "Flight ID: %1\n"
                           "Model: %2\n"
                           "Fuel: %3%\n"
                           "Origin: %4\n"
                           "Destination: %5\n"
                           "Status: %6\n"
                           "Priority: %7\n"
                           "Position: (%8, %9)\n"
                           "Waypoint Progress: %10/%11"
                           ).arg(QString::fromStdString(aircraft->flightID))
                           .arg(QString::fromStdString(aircraft->model))
                           .arg(aircraft->fuelLevel)
                           .arg(QString::fromStdString(aircraft->origin))
                           .arg(QString::fromStdString(aircraft->destination))
                           .arg(QString::fromStdString(aircraft->getStatusString()))
                           .arg(QString::fromStdString(aircraft->getPriorityString()))
                           .arg(aircraft->gridX)
                           .arg(aircraft->gridY)
                           .arg(aircraft->currentPathIndex)
                           .arg(aircraft->pathLength);

        QMessageBox::information(this, "Flight Information", info);
    } else {
        QMessageBox::warning(this, "Not Found", "Flight " + flightID + " not found!");
    }
}

void MainWindow::onShowFlightLog()
{
    QMessageBox::information(this, "Flight Log", "Check the console for chronological flight log.");
    flightLog->printLog();
}

void MainWindow::onFindRoute()
{
    QString flightID = QInputDialog::getText(this, "Find Route", "Enter Flight ID:");

    if(flightID.isEmpty()) return;

    Aircraft* aircraft = aircraftRegistry->search(flightID.toStdString());

    if(aircraft == nullptr) {
        QMessageBox::warning(this, "Not Found", "Flight not found!");
        return;
    }

    int currentNode = aircraft->currentNode;
    int nearestAirport = airspace->findNearestAirport(currentNode);

    if(nearestAirport == -1) {
        QMessageBox::warning(this, "Error", "No airport found!");
        return;
    }

    // Calculate safe route
    int pathLength = 0;
    int* path = airspace->calculatePath(currentNode, nearestAirport, pathLength);

    if(path == nullptr || pathLength == 0) {
        QMessageBox::warning(this, "No Route", "No safe route available!");
        return;
    }

    Node* current = airspace->getNode(currentNode);
    Node* nearest = airspace->getNode(nearestAirport);

    radarWidget->refresh();

    QString pathInfo = "Safe Route Found!\n\n";
    pathInfo += "From: " + QString::fromStdString(current->name) + "\n";
    pathInfo += "To: " + QString::fromStdString(nearest->name) + "\n";
    pathInfo += "Waypoints: " + QString::number(pathLength) + "\n\n";
    pathInfo += "Path: ";

    for(int i = 0; i < pathLength; i++) {
        Node* node = airspace->getNode(path[i]);
        if(node) {
            pathInfo += QString::fromStdString(node->name);
            if(i < pathLength - 1) pathInfo += " → ";
        }
    }

    delete[] path;

    logMessage("🗺️ Safe route displayed on radar for " + flightID, "blue");
    QMessageBox::information(this, "Safe Route", pathInfo);
}

void MainWindow::onSaveSystem()
{
    bool success = aircraftRegistry->saveToFile("aircraft_registry.txt") &&
                   flightLog->saveToFile("flight_logs.txt");

    if(success) {
        logMessage("💾 System state saved successfully!", "green");
        QMessageBox::information(this, "Save", "System state saved!");
    } else {
        logMessage("✗ Failed to save system state!", "red");
    }
}

void MainWindow::onLoadSystem()
{
    bool success = aircraftRegistry->loadFromFile("aircraft_registry.txt") ||
                   flightLog->loadFromFile("flight_logs.txt");

    if(success) {
        // Rebuild landing queue
        Aircraft** allAircraft;
        int count;
        aircraftRegistry->getAllAircraft(&allAircraft, count);

        for(int i = 0; i < count; i++) {
            if(allAircraft[i]->status == IN_FLIGHT ||
                allAircraft[i]->status == EMERGENCY) {
                landingQueue->insert(allAircraft[i]);
            }
        }

        delete[] allAircraft;

        logMessage("📂 System state loaded successfully!", "green");
        updateDisplay();
        QMessageBox::information(this, "Load", "System state loaded!");
    } else {
        logMessage("⚠ No saved data found!", "orange");
    }
}

void MainWindow::onRefreshRadar()
{
    radarWidget->refresh();
    logMessage("🔄 Radar refreshed", "gray");
}

void MainWindow::updateDisplay()
{
    updateQueueTable();
    updateAircraftTable();
    radarWidget->refresh();

    // Check for collisions and warn user
    if(radarWidget->hasCollisions()) {
        int collisions = radarWidget->getCollisionCount();
        logMessage("⚠️ WARNING: " + QString::number(collisions) +
                       " collision(s) detected on radar!", "red");
    }
}

void MainWindow::updateQueueTable()
{
    queueTable->setRowCount(0);

    MinHeap tempHeap(50);
    int size = landingQueue->getSize();
    Aircraft** aircraft = new Aircraft*[size];

    // Extract all and store
    for(int i = 0; i < size; i++) {
        aircraft[i] = landingQueue->extractMin();
    }

    // Display and re-insert
    for(int i = 0; i < size; i++) {
        queueTable->insertRow(i);
        queueTable->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        queueTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(aircraft[i]->flightID)));
        queueTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(aircraft[i]->getPriorityString())));
        queueTable->setItem(i, 3, new QTableWidgetItem(QString::number(aircraft[i]->fuelLevel) + "%"));

        landingQueue->insert(aircraft[i]);
    }

    delete[] aircraft;
}

void MainWindow::updateAircraftTable()
{
    aircraftTable->setRowCount(0);

    Aircraft** allAircraft;
    int count;
    aircraftRegistry->getAllAircraft(&allAircraft, count);

    for(int i = 0; i < count; i++) {
        aircraftTable->insertRow(i);
        aircraftTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(allAircraft[i]->flightID)));
        aircraftTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(allAircraft[i]->getStatusString())));
        aircraftTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(allAircraft[i]->getPriorityString())));
    }

    delete[] allAircraft;
}

void MainWindow::logMessage(const QString& message, const QString& color)
{
    QString html = QString("<span style='color:%1;'>%2</span><br>").arg(color, message);
    logOutput->append(html);
}

// RadarWidget Implementation
RadarWidget::RadarWidget(QWidget *parent)
    : QWidget(parent), airspace(nullptr), aircraftRegistry(nullptr),
    collisionCount(0), safePath(nullptr), safePathLength(0)
{
    setMinimumSize(GRID_SIZE * CELL_SIZE + 60, GRID_SIZE * CELL_SIZE + 80);
    setMaximumSize(GRID_SIZE * CELL_SIZE + 60, GRID_SIZE * CELL_SIZE + 80);

    // Initialize flight paths array
    plannedPathsCapacity = 10;
    plannedPathsCount = 0;
    plannedPaths = new FlightPath[plannedPathsCapacity];
    for(int i = 0; i < plannedPathsCapacity; i++) {
        plannedPaths[i].path = nullptr;
        plannedPaths[i].pathLength = 0;
    }

    clearGrid();
}

void RadarWidget::setPlannedPath(int* path, int length, const std::string& flightID)
{
    addPlannedPath(flightID, path, length);
}

void RadarWidget::addPlannedPath(const std::string& flightID, int* path, int length)
{
    // Check if path for this flight already exists
    for(int i = 0; i < plannedPathsCount; i++) {
        if(plannedPaths[i].flightID == flightID) {
            // Update existing path
            if(plannedPaths[i].path != nullptr) {
                delete[] plannedPaths[i].path;
            }
            plannedPaths[i].pathLength = length;
            plannedPaths[i].path = new int[length];
            for(int j = 0; j < length; j++) {
                plannedPaths[i].path[j] = path[j];
            }
            return;
        }
    }

    // Add new path
    if(plannedPathsCount >= plannedPathsCapacity) {
        // Expand capacity
        plannedPathsCapacity *= 2;
        FlightPath* newPaths = new FlightPath[plannedPathsCapacity];
        for(int i = 0; i < plannedPathsCount; i++) {
            newPaths[i] = plannedPaths[i];
        }
        for(int i = plannedPathsCount; i < plannedPathsCapacity; i++) {
            newPaths[i].path = nullptr;
            newPaths[i].pathLength = 0;
        }
        delete[] plannedPaths;
        plannedPaths = newPaths;
    }

    plannedPaths[plannedPathsCount].flightID = flightID;
    plannedPaths[plannedPathsCount].pathLength = length;
    plannedPaths[plannedPathsCount].path = new int[length];
    for(int i = 0; i < length; i++) {
        plannedPaths[plannedPathsCount].path[i] = path[i];
    }
    plannedPathsCount++;
}


void RadarWidget::removePlannedPath(const std::string& flightID)
{
    for(int i = 0; i < plannedPathsCount; i++) {
        if(plannedPaths[i].flightID == flightID) {
            // Delete the path
            if(plannedPaths[i].path != nullptr) {
                delete[] plannedPaths[i].path;
            }

            // Shift remaining paths
            for(int j = i; j < plannedPathsCount - 1; j++) {
                plannedPaths[j] = plannedPaths[j + 1];
            }

            plannedPathsCount--;
            plannedPaths[plannedPathsCount].path = nullptr;
            plannedPaths[plannedPathsCount].pathLength = 0;
            plannedPaths[plannedPathsCount].flightID = "";
            break;
        }
    }
}

void RadarWidget::clearPathForFlight(const std::string& flightID)
{
    removePlannedPath(flightID);
    refresh();
}

void RadarWidget::setSafePath(int* path, int length)
{
    if(safePath != nullptr) {
        delete[] safePath;
    }

    safePathLength = length;
    safePath = new int[length];
    for(int i = 0; i < length; i++) {
        safePath[i] = path[i];
    }
}

void RadarWidget::clearAllPaths()
{
    for(int i = 0; i < plannedPathsCount; i++) {
        if(plannedPaths[i].path != nullptr) {
            delete[] plannedPaths[i].path;
            plannedPaths[i].path = nullptr;
        }
    }
    plannedPathsCount = 0;

    if(safePath != nullptr) {
        delete[] safePath;
        safePath = nullptr;
    }
    safePathLength = 0;
}

void RadarWidget::setAirspace(Graph* graph)
{
    airspace = graph;
}

void RadarWidget::setRegistry(HashTable* registry)
{
    aircraftRegistry = registry;
}

void RadarWidget::clearGrid()
{
    for(int i = 0; i < GRID_SIZE; i++) {
        for(int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = '.';
        }
    }
}

void RadarWidget::placeAirports()
{
    if(!airspace) return;

    for(int i = 0; i < airspace->getNodeCount(); i++) {
        Node* node = airspace->getNode(i);
        if(node && node->isAirport) {
            int x = node->gridX;
            int y = node->gridY;
            if(x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
                grid[y][x] = 'A';
            }
        }
    }
}

void RadarWidget::placeAircraft()
{
    if(!aircraftRegistry) return;

    Aircraft** allAircraft;
    int count;
    aircraftRegistry->getAllAircraft(&allAircraft, count);

    for(int i = 0; i < count; i++) {
        if(allAircraft[i]->status == IN_FLIGHT ||
            allAircraft[i]->status == EMERGENCY ||
            allAircraft[i]->status == LANDING) {
            int x = allAircraft[i]->gridX;
            int y = allAircraft[i]->gridY;

            if(x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
                if(grid[y][x] == '.') {
                    grid[y][x] = 'P';
                } else if(grid[y][x] == 'P') {
                    grid[y][x] = 'X';
                }
            }
        }
    }

    delete[] allAircraft;
}

void RadarWidget::refresh()
{
    clearGrid();
    placeAirports();
    placeAircraft();
    update();
}

bool RadarWidget::hasCollisions() const
{
    return collisionCount > 0;
}

int RadarWidget::getCollisionCount() const
{
    return collisionCount;
}

void RadarWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), QColor(15, 15, 30));

    int offsetX = 20;
    int offsetY = 20;

    // Draw ALL planned paths FIRST (green)
    if(plannedPathsCount > 0 && airspace != nullptr) {
        drawPlannedPaths(painter);
    }

    // Draw the safe path SECOND (blue)
    if(safePath != nullptr && safePathLength > 0 && airspace != nullptr) {
        drawSafePath(painter);
    }

    // Draw grid lines
    painter.setPen(QPen(QColor(40, 40, 60), 1));
    for(int i = 0; i <= GRID_SIZE; i++) {
        int x = offsetX + i * CELL_SIZE;
        painter.drawLine(x, offsetY, x, offsetY + GRID_SIZE * CELL_SIZE);

        int y = offsetY + i * CELL_SIZE;
        painter.drawLine(offsetX, y, offsetX + GRID_SIZE * CELL_SIZE, y);
    }

    // Draw grid contents
    collisionCount = 0;
    for(int i = 0; i < GRID_SIZE; i++) {
        for(int j = 0; j < GRID_SIZE; j++) {
            int x = offsetX + j * CELL_SIZE;
            int y = offsetY + i * CELL_SIZE;

            char symbol = grid[i][j];

            if(symbol == 'A') {
                painter.setBrush(QColor(0, 200, 0, 150));
                painter.setPen(QPen(QColor(0, 255, 0), 2));
                painter.drawRect(x + 3, y + 3, CELL_SIZE - 6, CELL_SIZE - 6);
                painter.setBrush(QColor(0, 255, 0));
                painter.setPen(Qt::NoPen);
                painter.drawEllipse(x + 6, y + 6, CELL_SIZE - 12, CELL_SIZE - 12);

                painter.setPen(QColor(255, 255, 255));
                painter.setFont(QFont("Arial", 8, QFont::Bold));
                painter.drawText(QRect(x, y, CELL_SIZE, CELL_SIZE),
                                 Qt::AlignCenter, "A");

            } else if(symbol == 'P') {
                painter.setBrush(QColor(255, 255, 0));
                painter.setPen(QPen(QColor(255, 200, 0), 2));
                painter.drawEllipse(x + 4, y + 4, CELL_SIZE - 8, CELL_SIZE - 8);

                painter.setBrush(QColor(255, 200, 0));
                painter.setPen(Qt::NoPen);
                QPoint triangle[3] = {
                    QPoint(x + CELL_SIZE/2, y + 5),
                    QPoint(x + 5, y + CELL_SIZE - 5),
                    QPoint(x + CELL_SIZE - 5, y + CELL_SIZE - 5)
                };
                painter.drawPolygon(triangle, 3);

            } else if(symbol == 'X') {
                collisionCount++;
                painter.setPen(QPen(QColor(255, 0, 0), 4));
                painter.drawLine(x + 4, y + 4, x + CELL_SIZE - 4, y + CELL_SIZE - 4);
                painter.drawLine(x + CELL_SIZE - 4, y + 4, x + 4, y + CELL_SIZE - 4);

                painter.setBrush(QColor(255, 0, 0, 100));
                painter.setPen(Qt::NoPen);
                painter.drawEllipse(x + 2, y + 2, CELL_SIZE - 4, CELL_SIZE - 4);

                painter.setPen(QColor(255, 255, 255));
                painter.setFont(QFont("Arial", 7, QFont::Bold));
                painter.drawText(QRect(x, y + CELL_SIZE - 10, CELL_SIZE, 10),
                                 Qt::AlignCenter, "!");
            }
        }
    }

    // Draw coordinate labels
    painter.setPen(QColor(150, 150, 150));
    painter.setFont(QFont("Arial", 8));

    for(int i = 0; i <= GRID_SIZE; i += 5) {
        int x = offsetX + i * CELL_SIZE;
        painter.drawText(x - 10, offsetY - 5, 20, 15, Qt::AlignCenter, QString::number(i));
    }

    for(int i = 0; i <= GRID_SIZE; i += 5) {
        int y = offsetY + i * CELL_SIZE;
        painter.drawText(offsetX - 25, y - 7, 20, 15, Qt::AlignRight | Qt::AlignVCenter, QString::number(i));
    }

    // Legend
    int legendY = offsetY + GRID_SIZE * CELL_SIZE + 15;
    painter.setFont(QFont("Arial", 10, QFont::Bold));

    painter.setBrush(QColor(0, 255, 0));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(offsetX, legendY, 12, 12);
    painter.setPen(Qt::white);
    painter.drawText(offsetX + 20, legendY + 10, "Airport");

    painter.setBrush(QColor(255, 255, 0));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(offsetX + 100, legendY, 12, 12);
    painter.setPen(Qt::white);
    painter.drawText(offsetX + 120, legendY + 10, "Plane");

    painter.setPen(QPen(QColor(255, 0, 0), 3));
    painter.drawLine(offsetX + 200, legendY + 2, offsetX + 210, legendY + 10);
    painter.drawLine(offsetX + 210, legendY + 2, offsetX + 200, legendY + 10);
    painter.setPen(Qt::white);
    painter.drawText(offsetX + 220, legendY + 10, "Collision");

    if(plannedPathsCount > 0) {
        painter.setPen(QPen(QColor(0, 255, 0), 3));
        painter.drawLine(offsetX + 320, legendY + 6, offsetX + 340, legendY + 6);
        painter.setPen(Qt::white);
        painter.drawText(offsetX + 350, legendY + 10, "Planned");
    }

    if(safePathLength > 0) {
        painter.setPen(QPen(QColor(0, 150, 255), 3));
        painter.drawLine(offsetX + 430, legendY + 6, offsetX + 450, legendY + 6);
        painter.setPen(Qt::white);
        painter.drawText(offsetX + 460, legendY + 10, "Safe");
    }

    painter.setPen(QPen(QColor(100, 100, 150), 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(offsetX - 2, offsetY - 2,
                     GRID_SIZE * CELL_SIZE + 4, GRID_SIZE * CELL_SIZE + 4);
}

void RadarWidget::drawPlannedPaths(QPainter& painter)
{
    if(!airspace) return;

    int offsetX = 20;
    int offsetY = 20;

    // Draw all planned paths
    for(int p = 0; p < plannedPathsCount; p++) {
        if(plannedPaths[p].path == nullptr || plannedPaths[p].pathLength < 2) continue;

        // Draw GREEN path lines
        for(int i = 0; i < plannedPaths[p].pathLength - 1; i++) {
            Node* node1 = airspace->getNode(plannedPaths[p].path[i]);
            Node* node2 = airspace->getNode(plannedPaths[p].path[i + 1]);

            if(node1 && node2) {
                int x1 = offsetX + node1->gridX * CELL_SIZE + CELL_SIZE / 2;
                int y1 = offsetY + node1->gridY * CELL_SIZE + CELL_SIZE / 2;
                int x2 = offsetX + node2->gridX * CELL_SIZE + CELL_SIZE / 2;
                int y2 = offsetY + node2->gridY * CELL_SIZE + CELL_SIZE / 2;

                painter.setPen(QPen(QColor(0, 255, 0, 180), 3, Qt::SolidLine, Qt::RoundCap));
                painter.drawLine(x1, y1, x2, y2);

                if(i % 2 == 0) {
                    double angle = atan2(y2 - y1, x2 - x1);
                    int arrowSize = 6;

                    QPoint arrowHead[3];
                    arrowHead[0] = QPoint(x2, y2);
                    arrowHead[1] = QPoint(x2 - arrowSize * cos(angle - M_PI / 6),
                                          y2 - arrowSize * sin(angle - M_PI / 6));
                    arrowHead[2] = QPoint(x2 - arrowSize * cos(angle + M_PI / 6),
                                          y2 - arrowSize * sin(angle + M_PI / 6));

                    painter.setBrush(QColor(0, 255, 0, 180));
                    painter.setPen(Qt::NoPen);
                    painter.drawPolygon(arrowHead, 3);
                }
            }
        }

        // Draw waypoint markers
        for(int i = 0; i < plannedPaths[p].pathLength; i++) {
            Node* node = airspace->getNode(plannedPaths[p].path[i]);
            if(node) {
                int x = offsetX + node->gridX * CELL_SIZE + CELL_SIZE / 2;
                int y = offsetY + node->gridY * CELL_SIZE + CELL_SIZE / 2;

                if(i == 0) {
                    painter.setBrush(QColor(0, 255, 0, 220));
                    painter.setPen(QPen(Qt::white, 2));
                    painter.drawEllipse(x - 5, y - 5, 10, 10);
                } else if(i == plannedPaths[p].pathLength - 1) {
                    painter.setBrush(QColor(0, 200, 0, 220));
                    painter.setPen(QPen(Qt::white, 2));
                    painter.drawEllipse(x - 5, y - 5, 10, 10);
                } else {
                    painter.setBrush(QColor(100, 255, 100, 150));
                    painter.setPen(QPen(Qt::white, 1));
                    painter.drawEllipse(x - 3, y - 3, 6, 6);
                }
            }
        }
    }
}

void RadarWidget::drawSafePath(QPainter& painter)
{
    if(!airspace || safePath == nullptr || safePathLength < 2) return;

    int offsetX = 20;
    int offsetY = 20;

    for(int i = 0; i < safePathLength - 1; i++) {
        Node* node1 = airspace->getNode(safePath[i]);
        Node* node2 = airspace->getNode(safePath[i + 1]);

        if(node1 && node2) {
            int x1 = offsetX + node1->gridX * CELL_SIZE + CELL_SIZE / 2;
            int y1 = offsetY + node1->gridY * CELL_SIZE + CELL_SIZE / 2;
            int x2 = offsetX + node2->gridX * CELL_SIZE + CELL_SIZE / 2;
            int y2 = offsetY + node2->gridY * CELL_SIZE + CELL_SIZE / 2;

            painter.setPen(QPen(QColor(0, 150, 255, 200), 4, Qt::SolidLine, Qt::RoundCap));
            painter.drawLine(x1, y1, x2, y2);

            if(i % 2 == 0) {
                double angle = atan2(y2 - y1, x2 - x1);
                int arrowSize = 7;

                QPoint arrowHead[3];
                arrowHead[0] = QPoint(x2, y2);
                arrowHead[1] = QPoint(x2 - arrowSize * cos(angle - M_PI / 6),
                                      y2 - arrowSize * sin(angle - M_PI / 6));
                arrowHead[2] = QPoint(x2 - arrowSize * cos(angle + M_PI / 6),
                                      y2 - arrowSize * sin(angle + M_PI / 6));

                painter.setBrush(QColor(0, 150, 255, 200));
                painter.setPen(Qt::NoPen);
                painter.drawPolygon(arrowHead, 3);
            }
        }
    }

    for(int i = 0; i < safePathLength; i++) {
        Node* node = airspace->getNode(safePath[i]);
        if(node) {
            int x = offsetX + node->gridX * CELL_SIZE + CELL_SIZE / 2;
            int y = offsetY + node->gridY * CELL_SIZE + CELL_SIZE / 2;

            if(i == 0) {
                painter.setBrush(QColor(0, 255, 255, 220));
                painter.setPen(QPen(Qt::white, 2));
                painter.drawEllipse(x - 5, y - 5, 10, 10);
            } else if(i == safePathLength - 1) {
                painter.setBrush(QColor(0, 100, 255, 220));
                painter.setPen(QPen(Qt::white, 2));
                painter.drawEllipse(x - 5, y - 5, 10, 10);
            } else {
                painter.setBrush(QColor(100, 200, 255, 180));
                painter.setPen(QPen(Qt::white, 1));
                painter.drawEllipse(x - 3, y - 3, 6, 6);
            }
        }
    }
}
