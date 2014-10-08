#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "qcustomplot.h"
#include <iostream>
#include <sstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include <zmq.hpp>
#include "zhelpers.h"
#include "data_structs.h"
#include "groupwindows.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

  void setupRealtimeDataDemo(QCustomPlot *customPlot);
  void setupPlayground(QCustomPlot *customPlot);
  void setupGroup(GroupWindows *groupPlots);

  std::string isDataAvailable();

private slots:
  void realtimeDataSlot();
  void bracketDataSlot();

private:
  Ui::MainWindow *ui;
  QString demoName;
  QTimer dataTimer;
  QCPItemTracer *itemDemoPhaseTracer;
  int currentDemoIndex;

  void *m_zcontext;
  void *m_zsocket;
  double tmpValue;

  int iTickStep;
  int iWindowStep;

  std::shared_ptr<GroupWindows> groupPlots;

};

#endif // MAINWINDOW_H
