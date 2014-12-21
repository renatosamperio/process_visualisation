#include <QApplication>

#include <iostream>
#include <memory>
#include <vector>

#include "mainwindow.h"
// #include "groupwindows.h"
#include "remotedatafeeder.h"
// #include "Poco/Thread.h"
#include "Poco/ThreadPool.h"
#include "Poco/Runnable.h"

#include <qthread.h>
#include <qthreadpool.h>

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

      while (!dataObserver->isReceivingData());
      
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
//   int processSize = dataObserver->getData()->getProcessSize(); 
  
//   cout << "+ Number of monitoring processes: "<< processSize << endl;
   
  GUIinThread timeSeriesPlotter(dataObserver);
  
  Poco::ThreadPool::defaultPool().start(*dataObserver);
  
  Poco::ThreadPool::defaultPool().start(timeSeriesPlotter);
  Poco::ThreadPool::defaultPool().joinAll();
  
//   Poco::Thread thread;
//   thread.start(runnable);
//   thread.join();
 
//  while(true); // prevent main() from return
 return 0;
}
