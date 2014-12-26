#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <sstream>

#include <QMainWindow>
#include <QTimer>
#include <qrunnable.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include <zmq.hpp>

#include "qcustomplot.h"
#include "zhelpers.h"
#include "data_structs.h"
#include "remotedatafeeder.h"

enum PlotType 
{ 
	kMemory,
	kCPU,
	kMaxPlotTypes,
	kNone=-1
};

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
  void setupObserver(std::shared_ptr<RemoteDataFeeder> &, int, PlotType);


private slots:
  void timeSeriesFeederSlot();

private:
  Ui::MainWindow *	m_ui;
  QString 			m_windowProcessName;
  QTimer 			m_dataTimer;
  int 				m_procId;

  void *			m_zcontext;
  void *			m_zsocket;
  double 			m_tmpValue;

  int 				m_iTickStep;
  int 				m_iWindowStep;

  std::shared_ptr<RemoteDataFeeder> m_observer;
  
  PlotType		m_plotType;

};

#endif // MAINWINDOW_H
