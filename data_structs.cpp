#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

#include "data_structs.h"

ProcessInfo::ProcessInfo(){
  username          = "";
  status            = "";
  cpu_times         = "";
  memory_vms_label  = "";
  memory_rss_label  = "";
  name              = "";
  memory_vms_info   = INT_MIN;
  memory_rss_info   = INT_MIN;
  nice              = INT_MIN;
  cpu_percent       = -DBL_MAX;
  memory_percent    = -DBL_MAX;
}

ListProcessInfo::ListProcessInfo(){
  processSize       = INT_MIN;
}

void ListProcessInfo::decapsulate(std::string &message){
  namespace bpt = boost::property_tree;
  std::stringstream ss(message);
  bpt::ptree pt;
  bpt::read_json(ss, pt);

//  std::cout << "--- message [" << strip(message)<<"]"<<std::endl;
  BOOST_FOREACH(const bpt::ptree::value_type& child, pt.get_child("data") ) {
    std::shared_ptr<ProcessInfo> pData(new ProcessInfo);
    pData->name              = child.second.get<std::string>("name");
    pData->username          = child.second.get<std::string>("username");
    pData->status            = child.second.get<std::string>("status");
    pData->cpu_times         = child.second.get<std::string>("cpu_times");
    pData->memory_vms_label  = child.second.get<std::string>("memory_vms_label");
    pData->memory_rss_label  = child.second.get<std::string>("memory_rss_label");

    pData->memory_vms_info   = child.second.get<int>("memory_vms_info");
    pData->memory_rss_info   = child.second.get<int>("memory_rss_info");
    pData->nice              = child.second.get<int>("nice");

    pData->cpu_percent       = child.second.get<double>("cpu_percent");
    pData->memory_percent    = child.second.get<double>("memory_percent");

//    cout << "  *** name: " << pData->name << endl;
    lProcesses.push_back (  std::move(pData) );
  }
  processSize = lProcesses.size();
//  cout << "  *** processSize: " << processSize << endl;
}

std::string strip(const std::string &s, const std::string &chars) {
    size_t begin = 0;
    size_t end = s.size()-1;
    for(; begin < s.size(); begin++)
        if(chars.find_first_of(s[begin]) == string::npos)
            break;
    for(; end > begin; end--)
        if(chars.find_first_of(s[end]) == string::npos)
            break;
    return s.substr(begin, end-begin+1);
}

template <typename T>
string NumberToString ( T Number )
{
   ostringstream ss;
   ss << Number;
   return ss.str();
}

#endif // MAINWINDOW_H
