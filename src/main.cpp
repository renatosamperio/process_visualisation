#include <QApplication>

#include <iostream>
#include <memory>
#include <vector>

#include "mainwindow.h"
// #include "groupwindows.h"
#include "remotedatafeeder.h"


int main(int argc, char *argv[])
{
  std::string sSubEndpoint = "tcp://127.0.0.1:5563";
  std::shared_ptr<RemoteDataFeeder> 
    dataObserver( new RemoteDataFeeder(sSubEndpoint) );
  dataObserver->connect();
  int processSize = dataObserver->getData()->getProcessSize();
  cout << "+ Number of monitoring processes: "<< processSize << endl;

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  QApplication::setGraphicsSystem("raster");
#endif
  QApplication a(argc, argv);

  std::vector<std::shared_ptr<MainWindow>> my_plots;
  for (int i=0; i<processSize; i++)
  {
    std::shared_ptr<MainWindow> tmp(new MainWindow);
    my_plots.push_back( std::move(tmp) );
    my_plots[i]->setupObserver( dataObserver, i );
    my_plots[i]->show();
  }

  return a.exec();
}
