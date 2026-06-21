#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QPushButton>
#include <QTextEdit>
#include <QTableWidget>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include "Aircraft.h"
#include "Graph.h"
#include "MinHeap.h"
#include "HashTable.h"
#include "AVLTree.h"

class RadarWidget;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddFlight();
    void onDeclareEmergency();
    void onLandAircraft();
    void onSearchFlight();
    void onShowFlightLog();
    void onFindRoute();
    void onSaveSystem();
    void onLoadSystem();
    void onRefreshRadar();
    void onMoveAircraftStep();  // NEW: Step-by-step movement
    void updateDisplay();

private:
    Ui::MainWindow *ui;

    // Core components
    Graph* airspace;
    MinHeap* landingQueue;
    HashTable* aircraftRegistry;
    AVLTree* flightLog;

    // UI Components
    RadarWidget* radarWidget;
    QTableWidget* queueTable;
    QTableWidget* aircraftTable;
    QTextEdit* logOutput;
    QTimer* updateTimer;

    // Input fields
    QLineEdit* flightIDInput;
    QLineEdit* modelInput;
    QSpinBox* fuelInput;
    QComboBox* originCombo;
    QComboBox* destCombo;
    QComboBox* priorityCombo;
    QSpinBox* gridXInput;
    QSpinBox* gridYInput;

    // NEW: Movement control fields
    QLineEdit* moveFlightIDInput;
    QPushButton* moveStepBtn;

    void initializeSystem();
    void setupUI();
    void updateQueueTable();
    void updateAircraftTable();
    void logMessage(const QString& message, const QString& color = "black");
    void populateAirportCombos();
};

// Custom Widget for Radar Display
class RadarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RadarWidget(QWidget *parent = nullptr);
    void setAirspace(Graph* graph);
    void setRegistry(HashTable* registry);
    void refresh();
    bool hasCollisions() const;
    int getCollisionCount() const;
    void setPlannedPath(int* path, int length, const std::string& flightID);  // MODIFIED: Added flightID
    void setSafePath(int* path, int length);
    void clearAllPaths();
    void clearPathForFlight(const std::string& flightID);  // NEW: Clear specific flight path

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    static const int GRID_SIZE = 25;
    static const int CELL_SIZE = 20;
    char grid[GRID_SIZE][GRID_SIZE];
    Graph* airspace;
    HashTable* aircraftRegistry;
    int collisionCount;

    // Path highlighting - MODIFIED to support multiple flights
    struct FlightPath {
        std::string flightID;
        int* path;
        int pathLength;
    };

    FlightPath* plannedPaths;  // Array of flight paths
    int plannedPathsCount;
    int plannedPathsCapacity;

    int* safePath;         // Blue path (safe route)
    int safePathLength;

    void clearGrid();
    void placeAirports();
    void placeAircraft();
    void drawPlannedPaths(QPainter& painter);  // MODIFIED: Draw all paths
    void drawSafePath(QPainter& painter);
    void addPlannedPath(const std::string& flightID, int* path, int length);
    void removePlannedPath(const std::string& flightID);
};

#endif // MAINWINDOW_H
