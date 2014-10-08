#ifndef _DATA_STRUCTS_H_
#define _DATA_STRUCTS_H_

#include <QMainWindow>
#include <QTimer>
#include "qcustomplot.h"
#include <iostream>
#include <sstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <zmq.hpp>
#include "zhelpers.h"

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
    int memory_vms_info;
    int memory_rss_info;
    int nice;

};

class ListProcessInfo{
  public:
    ListProcessInfo();
    void decapsulate(std::string &message);

    inline int getProcessSize(){return processSize;}
    inline std::vector<ProcessInfo>& getLProcesses(){return lProcesses;};

  private:
    int processSize;
    std::vector<ProcessInfo> lProcesses;
};


std::string strip(const std::string& s, const std::string& chars=" ");
template <typename T> std::string NumberToString ( T Number );

#endif // _DATA_STRUCTS_H_
