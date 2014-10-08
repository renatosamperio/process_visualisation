#include <QApplication>

#include <iostream>
#include <memory>
#include <vector>

#include "mainwindow.h"
#include "groupwindows.h"


int main(int argc, char *argv[])
{
  std::string sSubEndpoint = "tcp://127.0.0.1:5563";
  GroupWindows groupPlots(sSubEndpoint);
  groupPlots.isDataAvailable();
  int processSize = groupPlots.getListProcess()->getProcessSize();

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  QApplication::setGraphicsSystem("raster");
#endif
  QApplication a(argc, argv);

  std::vector<std::shared_ptr<MainWindow>> my_plots;
  for (int i=0; i<processSize; i++){
    cout << "name: "<<groupPlots.getListProcess()->getLProcesses()[i].name <<endl;
////    unique_ptr<MainWindow>  tmp;
////    tmp.reset(new MainWindow);
    std::shared_ptr<MainWindow> tmp(new MainWindow);
    my_plots.push_back( tmp );
////    MainWindow w;
////    w.setupGroup(&groupPlots);
////    w.show();
  }
  my_plots[0]->show();
//  shared_ptr<MainWindow>  sp1;
//  sp1.reset(new MainWindow);
//  sp1->show();
//  MainWindow w;
//  w.show();
//  MainWindow w2;
//  w2.show();

    // Create a few new unique_ptr<Song> instances
    // and add them to vector using implicit move semantics.
//    songs.push_back(make_unique<Song>(L"B'z", L"Juice"));
  return a.exec();
}
