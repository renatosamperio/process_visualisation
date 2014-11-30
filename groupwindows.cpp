#include "groupwindows.h"

GroupWindows::GroupWindows(std::string endPoint):
  lProcesses(new ListProcessInfo)
{
  // Setting up a publisher
//  int iZMQ_sndhwm = 0;
//  string sPubEndpoint = "tcp://*:5563";
//  iResult = mtr_zipc::zipc_init_publisher (&m_zcontext,
//                                 &m_zsocket,
//                                 sPubEndpoint,
//                                 iZMQ_sndhwm);


//  *process_data =  process_data(new ListProcessInfo);

  // Preparing ZMQ subscriber
  int iResult = 0;iResult = iResult;
  int iZMQ_rcvhwm = 0;
  m_zcontext = zmq_ctx_new();
//  string sSubEndpoint = "tcp://127.0.0.1:5563";
  string sSubEndpoint = endPoint;

  m_zcontext = zmq_ctx_new ();
  m_zsocket = zmq_socket (m_zcontext, ZMQ_SUB);
  iResult = zmq_setsockopt(m_zsocket, ZMQ_RCVHWM, &iZMQ_rcvhwm, sizeof(iZMQ_rcvhwm));
  iResult = zmq_setsockopt(m_zsocket, ZMQ_SUBSCRIBE, "process_info", 0);
  zmq_connect (m_zsocket, sSubEndpoint.c_str() );
}

void GroupWindows::isDataAvailable(){
  // Obtaining ZMQ values
  zmq_msg_t msg;
//  cout << "*** Initialising socket with message..."<< endl;
  int rc = zmq_msg_init(&msg);
  assert(rc == 0);
//  cout << "*** Receiving message..."<< endl;
  int rcr = zmq_msg_recv (&msg, m_zsocket, 0);
  assert (rcr != -1);
//  cout << "*** Casting message in a string..."<< endl;
  string rpl = std::string(static_cast<char*>(zmq_msg_data(&msg)), zmq_msg_size(&msg));
  zmq_msg_close(&msg);
//  cout << "*** 1Closing message -" << rpl <<"-..."<< endl;
  string message_content = rpl.substr(rpl.find("{"), rpl.size());
//  cout << "*** 2Closing message [" << message_content <<"]..."<< endl;
//  cout << "*** Parsing message: -"<< message_content <<"-"<<endl;
  lProcesses->decapsulate(message_content);
}

GroupWindows::~GroupWindows()
{
  // Delete context and socket !!!
  int iResult = zmq_close(m_zsocket);
  iResult = zmq_ctx_destroy(m_zcontext);

  lProcesses.reset();
}
