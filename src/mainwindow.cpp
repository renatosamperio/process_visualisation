#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>

// ------------------------ Plot Index ------------------------
PlotIndex::PlotIndex()
{
	// Initiliasing indexes to original positions
	for (int i=0; i<kMaxPlotIds; i++)
	{
		indexes.push_back(i);
	}
}

inline void PlotIndex::setIndex(int plotId, int indexValue)
{
	indexes[plotId] = indexValue;
}

inline int PlotIndex::getIndex(int plotId)
{
	return indexes[plotId];
}

inline int PlotIndex::getNextIndex()
{
	int maxIndex = getIndex(kMaxPlotIds-1);
	for (int i=0; i<kMaxPlotIds; i++)
	{
		maxIndex = std::max(indexes[i],maxIndex);
	}
}

inline std::string PlotIndex::getIndexName(int plotId)
{
	std::string name = "";
	switch(plotId)
	{
		case kTimeSeries:
			name = "Time Series";
			break;
		case k1stDerivative:
			name = "1st Derivative";
			break;
		case kTimeSeriesDot:
			name = "Time Series dot";
			break;
		case k1stDerivativeDot:
			name = "1st Derivative dot";
			break;
		default:
			break;
	}
	return name;
}

// ------------------------ Main Window ------------------------
MainWindow::MainWindow(QWidget *parent) :
  m_procId(-1),
  QMainWindow(parent),
  m_iTickStep(100),
  m_iWindowStep(1000),
  m_tmpValue(0.0),
  m_ui(new Ui::MainWindow),
  m_plotType(kNone),
  m_was_delayed(false)
{

  //TODO: Make position, width and height depending on amount of graphs
  const int height =(  QApplication::desktop()->height()-100)/ 2;  
  const int width = QApplication::desktop()->width();
  
  m_ui->setupUi(this);
  setGeometry(400, 250, width, height);

  setupTimeSeriesPlot(m_ui->customPlot);
  statusBar()->clearMessage();

  move(0, 0);
  m_ui->customPlot->replot();

}

void MainWindow::addNewPlot(int plotId, Qt::GlobalColor plotColour, double value, int key)
{
	int nextIndTimeSeries = m_plotsIndexes.getNextIndex();
	m_plotsIndexes.setIndex(plotId, nextIndTimeSeries+1);
	cout << "  + Adding graph to ["<< m_plotsIndexes.getIndexName(plotId) <<"] in index ["<< m_plotsIndexes.getIndex(plotId)<<"]" << endl;
	m_ui->customPlot->addGraph(); // blue dot
	m_ui->customPlot->graph( m_plotsIndexes.getIndex(plotId) )->setPen(QPen(plotColour));
	m_ui->customPlot->graph( m_plotsIndexes.getIndex(plotId) )->addData(key, value);
	m_ui->customPlot->graph( m_plotsIndexes.getIndex(plotId) )->removeDataBefore (key);
}

void MainWindow::setupTimeSeriesPlot(QCustomPlot *customPlot)
{

//   m_graphInd=m_graphLastInd;
  customPlot->addGraph(); // blue line
  customPlot->graph( m_plotsIndexes.getIndex(kTimeSeries) )->setPen(QPen(Qt::blue));
  customPlot->addGraph(); // red line
  customPlot->graph( m_plotsIndexes.getIndex(k1stDerivative) )->setPen(QPen(Qt::red));
//   customPlot->graph(0)->setChannelFillGraph(customPlot->graph(1)); 

  customPlot->addGraph(); // blue dot
  customPlot->graph( m_plotsIndexes.getIndex(kTimeSeriesDot) )->setPen(QPen(Qt::blue));
  customPlot->graph( m_plotsIndexes.getIndex(kTimeSeriesDot) )->setLineStyle(QCPGraph::lsNone);
  customPlot->graph( m_plotsIndexes.getIndex(kTimeSeriesDot) )->setScatterStyle(QCPScatterStyle::ssDisc);
  customPlot->addGraph(); // red dot
  customPlot->graph( m_plotsIndexes.getIndex(k1stDerivativeDot) )->setPen(QPen(Qt::red));
  customPlot->graph( m_plotsIndexes.getIndex(k1stDerivativeDot) )->setLineStyle(QCPGraph::lsNone);
  customPlot->graph( m_plotsIndexes.getIndex(k1stDerivativeDot) )->setScatterStyle(QCPScatterStyle::ssDisc);

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

		std::shared_ptr<ProcessInfo> feedData = m_observer->data()->getProcess(m_procId);
		switch(m_plotType)
		{
			case kMemory:
				value0 = feedData->memory_vms_info;
				break;
			case kCPU:
				value0 = feedData->cpu_percent;
				break;
			default: 
				value0 = 0;
		}
		value1 = value0-m_tmpValue;
		
		// Time when message has not arrived
		if (m_observer->elapsed_time() > 3000000 )
		{

			m_was_delayed = true;
		}
		// Moment when message just arrived
		else if (m_was_delayed)
		{
			// Getting next index for adding plot
			addNewPlot(kTimeSeries, Qt::blue, value0, key);		
			addNewPlot(k1stDerivative, Qt::red, value1, key);
			m_was_delayed = false;
		}
		// Time when new message has arrived in less than 3s
		else
		{
			m_ui->customPlot->graph( m_plotsIndexes.getIndex(kTimeSeries) )->addData(key, value0);
			m_ui->customPlot->graph( m_plotsIndexes.getIndex(kTimeSeries) )->rescaleValueAxis();

			m_ui->customPlot->graph( m_plotsIndexes.getIndex(k1stDerivative) )->addData(key, value1);
			m_ui->customPlot->graph( m_plotsIndexes.getIndex(k1stDerivative) )->rescaleValueAxis(true);

			// Dot starting to plot
			m_ui->customPlot->graph( m_plotsIndexes.getIndex(kTimeSeriesDot) )->clearData();
			m_ui->customPlot->graph( m_plotsIndexes.getIndex(kTimeSeriesDot) )->addData(key, value0);

			m_ui->customPlot->graph( m_plotsIndexes.getIndex(k1stDerivativeDot) )->clearData();
			m_ui->customPlot->graph( m_plotsIndexes.getIndex(k1stDerivativeDot) )->addData(key, value1);
		}
		
		// Old values should be deleted
		m_ui->customPlot->graph( m_plotsIndexes.getIndex(kTimeSeries) )->removeDataBefore(key-m_iWindowStep);
		m_ui->customPlot->graph(m_plotsIndexes.getIndex(k1stDerivative))->removeDataBefore(key-m_iWindowStep);
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

	// Refreshing screen title and status status bar
	setupWindowTitle();
	
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
  setupWindowTitle();
  cout << "   + Assigning process information: ["<< m_procId <<", "<<m_plotType <<"]"<<endl;
}


void MainWindow::setupWindowTitle()
{
  QString windowProcessName = m_observer->data()->getProcess(m_procId)->name.c_str();
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
  setWindowTitle(windowProcessName + ": "+windowTitle.c_str() );
}

MainWindow::~MainWindow()
{
  delete m_ui;
}
