QT += core gui widgets

CONFIG += c++11

TARGET = SkyNetATC
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    Aircraft.cpp \
    Graph.cpp \
    MinHeap.cpp \
    HashTable.cpp \
    AVLTree.cpp \
    Radar.cpp
HEADERS += \
    mainwindow.h \
    Aircraft.h \
    Graph.h \
    MinHeap.h \
    HashTable.h \
    AVLTree.h \
    Radar.h
