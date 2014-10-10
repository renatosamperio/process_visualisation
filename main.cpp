#include <QApplication>

#include <iostream>
#include <memory>
#include <vector>

#include "mainwindow.h"
#include "groupwindows.h"


int main(int argc, char *argv[])
{
  std::string sSubEndpoint = "tcp://127.0.0.1:5563";
  std::shared_ptr<GroupWindows> groupPlots( new GroupWindows(sSubEndpoint) );
  groupPlots->isDataAvailable();
  int processSize = groupPlots->getListProcess()->getProcessSize();
  cout << "+ Number of monitoring processes: "<< processSize << endl;
  groupPlots.reset();

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  QApplication::setGraphicsSystem("raster");
#endif
  QApplication a(argc, argv);

  std::shared_ptr<ProcessInfo> processInfo (new ProcessInfo);

  std::vector<std::shared_ptr<MainWindow>> my_plots;
  for (int i=0; i<processSize; i++){
//    cout << "*** name: "<<groupPlots->getListProcess()->getLProcesses()[i]->name <<endl;
    std::shared_ptr<MainWindow> tmp(new MainWindow);
    my_plots.push_back( std::move(tmp) );
//    my_plots[i]->setupProcessInfo( groupPlots->getListProcess()->getLProcesses()[i] );
    my_plots[i]->setupProcessInfo( processInfo );
    my_plots[i]->show();
  }

  return a.exec();
}
