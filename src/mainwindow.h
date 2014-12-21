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
#include "remotedatafeeder.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

  void setupTimeSeriesPlot(QCustomPlot *customPlot);
  void setupPlayground(QCustomPlot *customPlot);
  void setupObserver(std::shared_ptr<RemoteDataFeeder> &, int id);


private slots:
  void timeSeriesFeederSlot();

private:
  Ui::MainWindow *ui;
  QString windowProcessName;
  QTimer dataTimer;
  QCPItemTracer *itemDemoPhaseTracer;
  int procId;

  void *m_zcontext;
  void *m_zsocket;
  double tmpValue;

  int iTickStep;
  int iWindowStep;

//   std::shared_ptr<ProcessInfo> processInfo;
  std::shared_ptr<RemoteDataFeeder> observer;
  
  int iPlotSize;

};

#endif // MAINWINDOW_H
