#ifndef _REMOTEDATAFEEDER_H_
#define _REMOTEDATAFEEDER_H_

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

class RemoteDataFeeder
{
public:
  explicit RemoteDataFeeder(std::string endPoint);
  ~RemoteDataFeeder();
  void isDataAvailable();
  void connect();

  inline std::shared_ptr<ListProcessInfo> getData(){ return lProcesses;}

private:
  void *m_zcontext;
  void *m_zsocket;
  double tmpValue;

  std::shared_ptr<ListProcessInfo> lProcesses;
};

#endif // _REMOTEDATAFEEDER_H_