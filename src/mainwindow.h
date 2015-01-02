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

enum PlotId
{
	kTimeSeries=0,
	k1stDerivative,
	kTimeSeriesDot,
	k1stDerivativeDot,
	kMaxPlotIds,
	kNonePlotId=-1	
};

class PlotIndex
{
public:
	PlotIndex();
	void setIndex(int plotId, int indexValue);
	int getIndex(int plotId);
	int getNextIndex();
	std::string getIndexName(int plotId);
private:
	std::vector<int> indexes;
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
  void setupWindowTitle();

  void addNewPlot(int plotId, Qt::GlobalColor plotColour, double value, int key);
private slots:
  void timeSeriesFeederSlot();

private:
  Ui::MainWindow *	m_ui;
  QTimer 			m_dataTimer;
  int 				m_procId;

  void *			m_zcontext;
  void *			m_zsocket;
  double 			m_tmpValue;

  int 				m_iTickStep;
  int 				m_iWindowStep;

  std::shared_ptr<RemoteDataFeeder> m_observer;
  
  PlotType			m_plotType;
  PlotIndex			m_plotsIndexes;

  bool 				m_was_delayed;
  double 			m_last_invalid_key;
  double 			m_last_valid_key;
};

#endif // MAINWINDOW_H
