#include "remotedatafeeder.h"

RemoteDataFeeder::RemoteDataFeeder(std::string endPoint):
  lProcesses(new ListProcessInfo)
{
  // Preparing ZMQ subscriber
  int iResult = 0;
  (void)iResult;
  int iZMQ_rcvhwm = 0;
  m_zcontext = zmq_ctx_new();
  string sSubEndpoint = endPoint;

  m_zcontext = zmq_ctx_new ();
  m_zsocket = zmq_socket (m_zcontext, ZMQ_SUB);
  iResult = zmq_setsockopt(m_zsocket, ZMQ_RCVHWM, &iZMQ_rcvhwm, sizeof(iZMQ_rcvhwm));
  iResult = zmq_setsockopt(m_zsocket, ZMQ_SUBSCRIBE, "process_info", 0);
  zmq_connect (m_zsocket, sSubEndpoint.c_str() );
}

void RemoteDataFeeder::connect(){
  zmq_msg_t msg;
  int rc = zmq_msg_init(&msg);
  assert(rc == 0);
  int rcr = zmq_msg_recv (&msg, m_zsocket, 0);
  assert (rcr != -1);
  string rpl = std::string(static_cast<char*>(zmq_msg_data(&msg)), zmq_msg_size(&msg));
  zmq_msg_close(&msg);
  string message_content = rpl.substr(rpl.find("{"), rpl.size());
  
  try
  {
    lProcesses->decapsulate(message_content);
  }
  
  catch(const boost::property_tree::ptree_bad_data &e)
  {
    cout << "-- -- -- -- Ptree Bad Data: " << e.what() << endl;
  }
  
  catch(const boost::property_tree::ptree_bad_path &e)
  {
    cout << "-- -- -- -- Ptree Bad Path: " << e.what() << endl;
  }
  catch(const boost::property_tree::ptree_error &e)
  {
    cout << "-- -- -- -- Ptree Error: " << e.what() << endl;
  }
}

void RemoteDataFeeder::isDataAvailable(){
  // Obtaining ZMQ values
  zmq_msg_t msg;
  int rc = zmq_msg_init(&msg);
  assert(rc == 0);
  int rcr = zmq_msg_recv (&msg, m_zsocket, 0);
  assert (rcr != -1);
  string rpl = std::string(static_cast<char*>(zmq_msg_data(&msg)), zmq_msg_size(&msg));
  zmq_msg_close(&msg);
  string message_content = rpl.substr(rpl.find("{"), rpl.size());
  try
  {
    lProcesses->decapsulate(message_content);
  }
  
  catch(const boost::property_tree::ptree_bad_data &e)
  {
    cout << "-- -- -- -- Ptree Bad Data: " << e.what() << endl;
  }
  
  catch(const boost::property_tree::ptree_bad_path &e)
  {
    cout << "-- -- -- -- Ptree Bad Path: " << e.what() << endl;
  }
  catch(const boost::property_tree::ptree_error &e)
  {
    cout << "-- -- -- -- Ptree Error: " << e.what() << endl;
  }
}

RemoteDataFeeder::~RemoteDataFeeder()
{
  // Delete context and socket !!!
  int iResult = zmq_close(m_zsocket);
  (void) iResult;
  iResult = zmq_ctx_destroy(m_zcontext);

  lProcesses.reset();
}
