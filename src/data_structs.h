#ifndef _DATA_STRUCTS_H_
#define _DATA_STRUCTS_H_

#include <iostream>
#include <sstream>
#include <memory>
#include <vector>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <QMainWindow>
#include <QTimer>
#include <zmq.hpp>

#include "zhelpers.h"
#include "qcustomplot.h"

#include "Poco/Thread.h"

class ProcessInfo{
  public:
    ProcessInfo();

    std::string username;
    std::string status;
    std::string cpu_times;
    std::string memory_vms_label;
    std::string memory_rss_label;
    std::string name;
    double cpu_percent;
    double memory_percent;
    long memory_vms_info;
    long memory_rss_info;
    int nice;
    int pid;

};

class ListProcessInfo{
  public:
    ListProcessInfo();
    void decapsulate(std::string &message);
    void decapsulate(std::string &message, int i);
    std::shared_ptr<ProcessInfo> getProcess(int id);

    inline int getProcessSize(){return processSize;}
    inline std::vector<std::shared_ptr<ProcessInfo> > getLProcesses(){ return lProcesses; }
	
private:
    int processSize;
	Poco::Mutex lock_;
    std::vector< std::shared_ptr<ProcessInfo> > lProcesses;
};


std::string strip(const std::string& s, const std::string& chars=" ");
template <typename T> std::string NumberToString ( T Number );

#endif // _DATA_STRUCTS_H_
