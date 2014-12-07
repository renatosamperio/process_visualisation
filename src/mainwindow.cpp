#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  // Setting up a publisher
//  int iZMQ_sndhwm = 0;
//  string sPubEndpoint = "tcp://*:5563";
//  iResult = mtr_zipc::zipc_init_publisher (&m_zcontext,
//                                 &m_zsocket,
//                                 sPubEndpoint,
//                                 iZMQ_sndhwm);

  // Initialisin plot variables
  tmpValue = 0.0;
  iTickStep= 100;
  iWindowStep = 1000;

  const int width = ( QApplication::desktop()->width()-10 ) /2;
  const int height =(  QApplication::desktop()->height()-100)/ 4;
//  std::cout << "*** ("<< height << ", "<< width << ")"<<std::endl;

  cout << " + Setting up QT window properties"<<endl;
  ui->setupUi(this);
  setGeometry(400, 250, width, height);

  setupRealtimeDataDemo(ui->customPlot);
  setWindowTitle("Process Memory: "+demoName);
  statusBar()->clearMessage();

  move(20, 30);
  ui->customPlot->replot();

}

void MainWindow::setupRealtimeDataDemo(QCustomPlot *customPlot)
{
#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
  QMessageBox::critical(this, "", "You're using Qt < 4.7, the realtime data demo needs functions that are available with Qt 4.7 to work properly");
#endif
  demoName = "Real time process data analyser";

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
//  cout << "iTickStep: "<< iTickStep<<endl;
  customPlot->xAxis->setTickStep(iTickStep);
  customPlot->axisRect()->setupFullAxesBox();

  // make left and bottom axes transfer their ranges to right and top axes:
  connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
  connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

  // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
  connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
  dataTimer.start(0); // Interval 0 means to refresh as fast as possible
}

void MainWindow::realtimeDataSlot()
{
  // calculate two new data points:
#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
  double key = 0;
#else
  double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
#endif
  static double lastPointKey = 0;
//  int windowStep = 100;
  // If new data is ready
   if (key-lastPointKey > 0.01) // at most add point every 10 ms
//  if (key-lastPointKey > 1) // at most add point every 1000 ms
  {
//    double value0 = qSin(key);
//    double value0 = qSin(key)*10;
//    if (value0 < 9 && value0 > 8)
//        value0 = 0;
//    std::string message = NumberToString ( value0 );
//    std::cout << "*** key: ("<<key<<", "<< message<<")"<<std::endl;
//    int size = zmq_send(m_zsocket, message.c_str(), strlen(message.c_str()), ZMQ_DONTWAIT);

    double value0 = 0.0;
    double value1 = 0.0;
    observer->isDataAvailable();
    
    std::shared_ptr<ListProcessInfo> list = observer->getListProcess();
    std::vector<std::shared_ptr<ProcessInfo> > listProcs = list->getLProcesses();
    int iSize = list->getProcessSize();
    for (int i=0; i<iSize; i++){
      value0 = listProcs[i]->memory_vms_info;
      value1 = value0-tmpValue;
//       cout << "*** name: "<<listProcs[i]->name<<endl;
//       cout << "*** memory_vms_info: "<<value0<<endl;
    }
    // Obtaining ZMQ values
//    zmq_msg_t msg;
//    int rc = zmq_msg_init(&msg);
//    assert(rc == 0);
//    int rcr = zmq_msg_recv (&msg, m_zsocket, 0);
//    assert (rcr != -1);
//    string rpl = strip(std::string(static_cast<char*>(zmq_msg_data(&msg)), zmq_msg_size(&msg)) );
//    zmq_msg_close(&msg);

//    string message = isDataAvailable();
//    cout << "*** ["<< processInfo->name <<"]: "<< message << endl;
//    ListProcessInfo process_data;
//    process_data.decapsulate(message);
//    cout << "*** processSize: "<< process_data.processSize << endl;
//    std::cout << "memory_vms_info:"  << process_data.lProcesses[0].memory_vms_info << std::endl;
//    std::cout << "memory_rss_info:"  << process_data.lProcesses[0].memory_rss_info << std::endl;
//    value0 = process_data.lProcesses[0].memory_vms_info;
//    value1 = value0-tmpValue;
//    std::cout << "memory_vms_info:"  << value0 << std::endl;
//    std::cout << "derivative:"  << value1 << std::endl;
//    cout << "RECV: "  << newValue<< " -> "<< value1 << endl;

    ui->customPlot->graph(0)->addData(key, value0);
    ui->customPlot->graph(1)->addData(key, value1);
    ui->customPlot->graph(2)->clearData();
    ui->customPlot->graph(2)->addData(key, value0);
    ui->customPlot->graph(3)->clearData();
    ui->customPlot->graph(3)->addData(key, value1);
    ui->customPlot->graph(0)->removeDataBefore(key-iWindowStep);
    ui->customPlot->graph(1)->removeDataBefore(key-iWindowStep);
    ui->customPlot->graph(0)->rescaleValueAxis();
    ui->customPlot->graph(1)->rescaleValueAxis(true);
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

void MainWindow::bracketDataSlot()
{
#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
  double secs = 0;
#else
  double secs = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
#endif

  // update data to make phase move:
  int n = 500;
  double phase = secs*5;
  double k = 3;
  QVector<double> x(n), y(n);
  for (int i=0; i<n; ++i)
  {
    x[i] = i/(double)(n-1)*34 - 17;
    y[i] = qExp(-x[i]*x[i]/20.0)*qSin(k*x[i]+phase);
  }
  ui->customPlot->graph()->setData(x, y);

  itemDemoPhaseTracer->setGraphKey((8*M_PI+fmod(M_PI*1.5-phase, 6*M_PI))/k);

  ui->customPlot->replot();

  // calculate frames per second:
  double key = secs;
  static double lastFpsKey;
  static int frameCount;
  ++frameCount;
  if (key-lastFpsKey > 2) // average fps over 2 seconds
  {
    ui->statusBar->showMessage(
          QString("%1 FPS, Total Data points: %2")
          .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
          .arg(ui->customPlot->graph(0)->data()->count())
          , 0);
    lastFpsKey = key;
    frameCount = 0;
  }
}

// void MainWindow::setupProcessInfo( std::shared_ptr<ProcessInfo> procInfo )
void MainWindow::setupObserver( std::shared_ptr<GroupWindows> & obs )
//void MainWindow::setupGroup(GroupWindows *grpPlots)
{
  cout << " + Assigning process information"<<endl;
  observer = obs;
//   std::shared_ptr<ListProcessInfo> list = observer->getListProcess();
//   std::vector<std::shared_ptr<ProcessInfo> > listProcs = list->getLProcesses();
//   int iSize = list->getProcessSize();
//   for (int i=0; i<iSize; i++){
//     cout << "*** name: "<<listProcs[i]->name<<endl;
//   }
}

std::string MainWindow::isDataAvailable(){
  // Obtaining ZMQ values
//  zmq_msg_t msg;
//  int rc = zmq_msg_init(&msg);
//  assert(rc == 0);
//  int rcr = zmq_msg_recv (&msg, m_zsocket, 0);
//  assert (rcr != -1);
//  string rpl = strip(std::string(static_cast<char*>(zmq_msg_data(&msg)), zmq_msg_size(&msg)) );
//  zmq_msg_close(&msg);
//  string message_content = rpl.substr(rpl.find("{"));
//  return message_content;
return std::string("");
}

MainWindow::~MainWindow()
{
  delete ui;
}
