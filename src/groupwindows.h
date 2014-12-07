#ifndef _GROUPWINDOWS_H_
#define _GROUPWINDOWS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sched.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#ifdef __cplusplus
}
#endif

#include <iostream>
#include <sstream>
#include <memory>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <zmq.hpp>
#include "zhelpers.h"

#include "data_structs.h"

class GroupWindows
{
public:
  explicit GroupWindows(std::string endPoint);
  ~GroupWindows();
  void isDataAvailable();
  void connect();

  inline std::shared_ptr<ListProcessInfo> getListProcess(){ return lProcesses;}

private:
  void *m_zcontext;
  void *m_zsocket;
  double tmpValue;

  std::shared_ptr<ListProcessInfo> lProcesses;
};

#endif // _GROUPWINDOWS_H_
