#include "remotedatafeeder.h"

RemoteDataFeeder::RemoteDataFeeder(std::string endPoint):
  lProcesses(new ListProcessInfo),
  bLoopCtrl(true),
  waitingTime(0.0)
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

RemoteDataFeeder::~RemoteDataFeeder()
{
  // Delete context and socket !!!
  int iResult = zmq_close(m_zsocket);
  (void) iResult;
  iResult = zmq_ctx_destroy(m_zcontext);

  lProcesses.reset();
}

void RemoteDataFeeder::run()
{
  //TODO: Log entries from catch's
  while (bLoopCtrl)
  {
    // Preparing message for ZMQ socket
    zmq_msg_t msg;
    int rc = zmq_msg_init(&msg);
    assert(rc == 0);
    int rcr = zmq_msg_recv (&msg, m_zsocket, ZMQ_NOBLOCK);
    size_t iMsgSize = zmq_msg_size(&msg);
    
    // If something has been received, parsed it
    if (iMsgSize > 0)
    {
		string rpl = std::string(static_cast<char*>(
		zmq_msg_data(&msg)), iMsgSize);
		zmq_msg_close(&msg);
		string message_content = rpl.substr(rpl.find("{"), rpl.size());

		// Parsing incoming message 
		try
		{
			lock_.lock();
			lProcesses->decapsulate(message_content);
			lock_.unlock();
		}
		catch(const boost::property_tree::ptree_bad_data &e)
		{
			hasStarted = false;
			cout << "-- -- -- -- Ptree Bad Data: " << e.what() << endl;
		}
		catch(const boost::property_tree::ptree_bad_path &e)
		{
			hasStarted = false;
			cout << "-- -- -- -- Ptree Bad Path: " << e.what() << endl;
		}
		catch(const boost::property_tree::ptree_error &e)
		{
			hasStarted = false;
			cout << "-- -- -- -- Ptree Error: " << e.what() << endl;
		}
      
		hasStarted = true;
		waitingTime = last_message_timer.elapsed();
		last_message_timer.restart();
    }
    else
		waitingTime = std::max(waitingTime, static_cast<double>(last_message_timer.elapsed()) );
    _event.tryWait(100);
  }
}

bool RemoteDataFeeder::isReceivingData()
{
  
  lock_.lock();
  bool reply = hasStarted;
  lock_.unlock();
  return reply;
  
  //TODO: Calculate a time since last available message was received
}

std::shared_ptr<ListProcessInfo> RemoteDataFeeder::data()
{ 
  lock_.lock();
  std::shared_ptr<ListProcessInfo> refProcesses = lProcesses;
  lock_.unlock();
  return refProcesses;
}

