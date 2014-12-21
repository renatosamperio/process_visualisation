#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>


MainWindow::MainWindow(QWidget *parent) :
  procId(-1),
  QMainWindow(parent),
  iTickStep(100),
  iWindowStep(1000),
  tmpValue(0.0),
  ui(new Ui::MainWindow),
  iPlotSize(1)
{

  const int width = ( QApplication::desktop()->width()-10 ) /2;
  const int height =(  QApplication::desktop()->height()-100)/ 4;
  
  ui->setupUi(this);
  setGeometry(400, 250, width, height);

  setupTimeSeriesPlot(ui->customPlot);
  statusBar()->clearMessage();

  move(0, 10);
  ui->customPlot->replot();

  observer->join();
}

void MainWindow::setupTimeSeriesPlot(QCustomPlot *customPlot)
{
#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
  QMessageBox::critical(this, "", "You're using Qt < 4.7, the realtime data demo needs functions that are available with Qt 4.7 to work properly");
#endif

  customPlot->addGraph(); // blue line
  customPlot->graph(0)->setPen(QPen(Qt::blue));
  customPlot->addGraph(); // red line
  customPlot->graph(1)->setPen(QPen(Qt::red));
  customPlot->graph(0)->setChannelFillGraph(customPlot->graph(1));

  customPlot->addGraph(); // blue dot
  customPlot->graph(2)->setPen(QPen(Qt::blue));
  customPlot->graph(2)->setLineStyle(QCPGraph::lsNone);
  customPlot->graph(2)->setScatterStyle(QCPScatterStyle::ssDisc);
  customPlot->addGraph(); // red dot
  customPlot->graph(3)->setPen(QPen(Qt::red));
  customPlot->graph(3)->setLineStyle(QCPGraph::lsNone);
  customPlot->graph(3)->setScatterStyle(QCPScatterStyle::ssDisc);

  customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
  customPlot->xAxis->setDateTimeFormat("hh:mm:ss");
  customPlot->xAxis->setAutoTickStep(false);
  customPlot->xAxis->setTickStep(iTickStep);
  customPlot->axisRect()->setupFullAxesBox();

  // make left and bottom axes transfer their ranges to right and top axes:
  connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
  connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

  connect(&dataTimer, SIGNAL(timeout()), this, SLOT(timeSeriesFeederSlot()));
  dataTimer.start(0); }

void MainWindow::timeSeriesFeederSlot()
{
  // calculate two new data points:
#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
  double key = 0;
#else
  double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
#endif
  static double lastPointKey = 0;
   if (key-lastPointKey > 0.01 && procId >= 0) // at most add point every 10 ms
  {
    double value0 = 0.0;
    double value1 = 0.0;
//     observer->isDataAvailable();
    std::vector<std::shared_ptr<ProcessInfo>> lData = observer->getData()->getLProcesses();
    value0 = lData[procId]->memory_vms_info;
    value1 = value0-tmpValue;
//     cout << "  - Requesting data ["<< value0<<", "<< value1 <<"]" << endl;

    ui->customPlot->graph(0)->addData(key, value0);
    ui->customPlot->graph(0)->removeDataBefore(key-iWindowStep);
    ui->customPlot->graph(0)->rescaleValueAxis();
    
    ui->customPlot->graph(1)->addData(key, value1);
    ui->customPlot->graph(1)->removeDataBefore(key-iWindowStep);
    ui->customPlot->graph(1)->rescaleValueAxis(true);

    // Dot starting to plot
//     ui->customPlot->graph(2)->clearData();
//     ui->customPlot->graph(2)->addData(key, value0);

//     ui->customPlot->graph(3)->clearData();
//     ui->customPlot->graph(3)->addData(key, value1);
    lastPointKey = key;

    tmpValue = value0;
  }
  // make key axis range scroll with the data (at a constant range size of 8):
  ui->customPlot->xAxis->setRange(key+0.25, iWindowStep, Qt::AlignRight);
  ui->customPlot->replot();

  // calculate frames per second:
  static double lastFpsKey;
  static int frameCount;
  ++frameCount;
  if (key-lastFpsKey > 2) // average fps over 2 seconds
  {
    ui->statusBar->showMessage(
          QString("%1 FPS, Total Data points: %2")
          .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
          .arg(ui->customPlot->graph(0)->data()->count()+ui->customPlot->graph(1)->data()->count())
          , 0);
    lastFpsKey = key;
    frameCount = 0;
  }
}

// void MainWindow::setupProcessInfo( std::shared_ptr<ProcessInfo> procInfo )
void MainWindow::setupObserver( std::shared_ptr<RemoteDataFeeder> & obs, int id )
{
  cout << " + Assigning process information: ["<< id <<"]"<<endl;
  observer = obs;
  procId = id;
  windowProcessName = observer->getData()->getProcess(id)->name.c_str();
  
  setWindowTitle(windowProcessName + " Virtual Memory");
}
 

MainWindow::~MainWindow()
{
  delete ui;
}
