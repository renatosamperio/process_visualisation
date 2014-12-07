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
  memory_vms_info   = -1;
  memory_rss_info   = -1;
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
  lProcesses.clear();

//  std::cout << "--- message  [" << strip(message)<<"]"<<std::endl;
  BOOST_FOREACH(const bpt::ptree::value_type& child, pt.get_child("data") ) {
    std::shared_ptr<ProcessInfo> pData(new ProcessInfo);
    pData->name              = child.second.get<std::string>("name");
// std::cout << "---    name:\t\t\t" << pData->name << std::endl;
    pData->username          = child.second.get<std::string>("username");
// std::cout << "---    username:\t\t" << pData->username << std::endl;
    pData->status            = child.second.get<std::string>("status");
// std::cout << "---    status:\t\t\t" << pData->status << std::endl;
    pData->cpu_times         = child.second.get<std::string>("cpu_times");
// std::cout << "---    cpu_times:\t\t" << pData->cpu_times << std::endl;
    pData->memory_vms_label  = child.second.get<std::string>("memory_vms_label");
// std::cout << "---    memory_vms_label:\t" << pData->memory_vms_label << std::endl;
    pData->memory_rss_label  = child.second.get<std::string>("memory_rss_label");
// std::cout << "---    memory_rss_label:\t" << pData->memory_rss_label << std::endl;

    pData->memory_vms_info   = child.second.get<long>("memory_vms_info");
// std::cout << "---    memory_vms_info:\t\t" << pData->memory_vms_info << std::endl;
    pData->memory_rss_info   = child.second.get<long>("memory_rss_info");
// std::cout << "---    memory_rss_info:\t\t" << pData->memory_rss_info << std::endl;
    pData->nice              = child.second.get<int>("nice");
// std::cout << "---    nice:\t\t\t" << pData->nice << std::endl;

    pData->cpu_percent       = child.second.get<double>("cpu_percent");
// std::cout << "---    cpu_percent:\t\t" << pData->cpu_percent << std::endl;
    pData->memory_percent    = child.second.get<double>("memory_percent");
// std::cout << "---    memory_percent:\t\t" << pData->memory_percent << std::endl;

//    cout << "  *** name: " << pData->name << endl;
    lProcesses.push_back (  std::move(pData) );
  }
  processSize = lProcesses.size();
//  cout << "  *** processSize: " << processSize << endl;
}

void ListProcessInfo::decapsulate(std::string &message, int id){
  namespace bpt = boost::property_tree;
  std::stringstream ss(message);
  bpt::ptree pt;
  bpt::read_json(ss, pt);
  lProcesses.clear();

 std::cout << "--- message ("<< id << ") [" << strip(message)<<"]"<<std::endl;
  BOOST_FOREACH(const bpt::ptree::value_type& child, pt.get_child("data") ) {
    std::shared_ptr<ProcessInfo> pData(new ProcessInfo);
    pData->name              = child.second.get<std::string>("name");
std::cout << "---    name:\t\t\t" << pData->name << std::endl;
    pData->username          = child.second.get<std::string>("username");
std::cout << "---    username:\t\t" << pData->username << std::endl;
    pData->status            = child.second.get<std::string>("status");
std::cout << "---    status:\t\t\t" << pData->status << std::endl;
    pData->cpu_times         = child.second.get<std::string>("cpu_times");
std::cout << "---    cpu_times:\t\t" << pData->cpu_times << std::endl;
    pData->memory_vms_label  = child.second.get<std::string>("memory_vms_label");
std::cout << "---    memory_vms_label:\t" << pData->memory_vms_label << std::endl;
    pData->memory_rss_label  = child.second.get<std::string>("memory_rss_label");
std::cout << "---    memory_rss_label:\t" << pData->memory_rss_label << std::endl;

    pData->memory_vms_info   = child.second.get<long>("memory_vms_info");
std::cout << "---    memory_vms_info:\t\t" << pData->memory_vms_info << std::endl;
    pData->memory_rss_info   = child.second.get<long>("memory_rss_info");
std::cout << "---    memory_rss_info:\t\t" << pData->memory_rss_info << std::endl;
    pData->nice              = child.second.get<int>("nice");
std::cout << "---    nice:\t\t\t" << pData->nice << std::endl;

    pData->cpu_percent       = child.second.get<double>("cpu_percent");
std::cout << "---    cpu_percent:\t\t" << pData->cpu_percent << std::endl;
    pData->memory_percent    = child.second.get<double>("memory_percent");
std::cout << "---    memory_percent:\t\t" << pData->memory_percent << std::endl;

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
