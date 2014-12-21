#include <QApplication>

#include <iostream>
#include <memory>
#include <vector>

#include "mainwindow.h"
#include "remotedatafeeder.h"
#include "Poco/ThreadPool.h"
#include "Poco/Runnable.h"

class GUIinThread : public Poco::Runnable
{
  public:
    GUIinThread(std::shared_ptr<RemoteDataFeeder> observer):
    dataObserver(observer)
    {
    }
    
    virtual void run()
    {
      int argc = 1;
      QByteArray argvStr = QString("").toLatin1();
      char* argv = argvStr.data();
  
      QApplication a(argc, &argv);

      // Wait until data is available to run time series timeSeriesPlotter
      while (!dataObserver->isReceivingData());
      
      // Setting up time series plotter with new incoming data feeds
      int processSize = dataObserver->getData()->getProcessSize(); 
      std::vector<std::shared_ptr<MainWindow>> my_plots;
      for (int i=0; i<processSize; i++)
      {
	std::shared_ptr<MainWindow> tmp(new MainWindow);
	my_plots.push_back( std::move(tmp) );
	my_plots[i]->setupObserver( dataObserver, i );
	cout << "  - Showing plot widget" << endl;
	my_plots[i]->show();
      }
     
      a.exec();
    }
  
  private:
    std::shared_ptr<RemoteDataFeeder> dataObserver;
};
 
int main(int argc, char *argv[])
{
  std::string sSubEndpoint = "tcp://127.0.0.1:5563";
  std::shared_ptr<RemoteDataFeeder> dataObserver( new RemoteDataFeeder(sSubEndpoint) );
   
  GUIinThread timeSeriesPlotter(dataObserver);
  
  Poco::ThreadPool::defaultPool().start(*dataObserver);
  Poco::ThreadPool::defaultPool().start(timeSeriesPlotter);
  Poco::ThreadPool::defaultPool().joinAll();
  
  return 0;
}
