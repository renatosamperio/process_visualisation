#
#  QCustomPlot Plot Examples
#

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = rt-graph
TEMPLATE = app

SOURCES += main.cpp\
           mainwindow.cpp \
           groupwindows.cpp \
           data_structs.cpp \
           qcustomplot.cpp

HEADERS  += mainwindow.h \
            groupwindows.h \
            data_structs.h \
            qcustomplot.h

FORMS    += mainwindow.ui

LIBS += -lzmq

QMAKE_CXXFLAGS += -std=c++0x
