#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>


MainWindow::MainWindow(QWidget *parent) :
  m_procId(-1),
  QMainWindow(parent),
  m_iTickStep(100),
  m_iWindowStep(1000),
  m_tmpValue(0.0),
  m_ui(new Ui::MainWindow),
  m_plotType(kNone)
{

  //TODO: Make position, width and height depending on amount of graphs
  const int width = ( QApplication::desktop()->width()-10 ) /2;
  const int height =(  QApplication::desktop()->height()-100)/ 2;
  
  m_ui->setupUi(this);
  setGeometry(400, 250, width, height);

  setupTimeSeriesPlot(m_ui->customPlot);
  statusBar()->clearMessage();

  move(0, 10);
  m_ui->customPlot->replot();

}

void MainWindow::setupTimeSeriesPlot(QCustomPlot *customPlot)
{
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
  customPlot->xAxis->setTickStep(m_iTickStep);
  customPlot->axisRect()->setupFullAxesBox();

  // make left and bottom axes transfer their ranges to right and top axes:
  connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
  connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

  connect(&m_dataTimer, SIGNAL(timeout()), this, SLOT(timeSeriesFeederSlot()));
  m_dataTimer.start(0); 

}

void MainWindow::timeSeriesFeederSlot()
{
	// calculate two new data points:
	double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
	
	static double lastPointKey = 0;
	// at most add point every 100 ms
	if (key-lastPointKey > 0.1 && m_procId >= 0) 
	{
		double value0 = 0.0;
		double value1 = 0.0;

		std::vector<std::shared_ptr<ProcessInfo>> lData = m_observer->data()->getLProcesses();
		
		switch(m_plotType)
		{
			case kMemory:
				value0 = lData[m_procId]->memory_vms_info;
				break;
			case kCPU:
				value0 = lData[m_procId]->cpu_percent;
				break;
			default: 
				value0 = 0;
		}
		value1 = value0-m_tmpValue;

		m_ui->customPlot->graph(0)->addData(key, value0);
		m_ui->customPlot->graph(0)->removeDataBefore(key-m_iWindowStep);
		m_ui->customPlot->graph(0)->rescaleValueAxis();

		m_ui->customPlot->graph(1)->addData(key, value1);
		m_ui->customPlot->graph(1)->removeDataBefore(key-m_iWindowStep);
		m_ui->customPlot->graph(1)->rescaleValueAxis(true);

		// Dot starting to plot
		m_ui->customPlot->graph(2)->clearData();
		m_ui->customPlot->graph(2)->addData(key, value0);

		m_ui->customPlot->graph(3)->clearData();
		m_ui->customPlot->graph(3)->addData(key, value1);

		m_tmpValue = value0;
		lastPointKey = key;
			
	}
	// make key axis range scroll with the data (at a constant range size of 8):
	m_ui->customPlot->xAxis->setRange(key+0.25, m_iWindowStep, Qt::AlignRight);
	m_ui->customPlot->replot();

	// calculate frames per second:
	static double lastFpsKey;
	static int frameCount;
	++frameCount;

	if (key-lastFpsKey > 1) // average fps over 1 second(s)
	{
		double waitingTime = m_observer->waiting_time();

		QString status_message = QString("%1 FPS, Total Data points: %2")
				.arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
				.arg(m_ui->customPlot->graph(0)->data()->count()+m_ui->customPlot->graph(1)->data()->count());
		status_message += QString("                                                                    ");
		status_message += QString("Time since last message: %1 s")
				.arg( QString::number( waitingTime/ 1000000, 'f', 2 ) );
			
		lastFpsKey = key;
		frameCount = 0;
		m_ui->statusBar->showMessage(status_message, 0);
	}
}

void MainWindow::setupObserver( std::shared_ptr<RemoteDataFeeder> & obs, 
								int id,
								PlotType plotType
  							)
{
  m_observer = obs;
  m_procId = id;
  m_plotType = plotType;
  m_windowProcessName = m_observer->data()->getProcess(id)->name.c_str();
  cout << "   + Assigning process information: ["<< m_procId <<", "<<m_plotType <<"]"<<endl;
  
  // Setting label of window name
  std::string windowTitle("");
  switch(m_plotType)
  {
	  case kMemory:
		  windowTitle = "total memory";
		  m_ui->customPlot->setYAxisUnits(QCPAxis::LabelType::ltBytes);
		  break;
	  case kCPU:
		  windowTitle = "CPU usage";
		  m_ui->customPlot->setYAxisUnits(QCPAxis::LabelType::lPercentage);
		  break;
	  default: break;
  }
  setWindowTitle(m_windowProcessName + ": "+windowTitle.c_str() );
}
 
MainWindow::~MainWindow()
{
  delete m_ui;
}
