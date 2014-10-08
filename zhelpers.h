#ifndef _ZHELPERS_H_
#define _ZHELPERS_H_
///////////////////////////////////////////////////////////////////////////////
//
//         Name: zehlpers.h
//      Project: MTR Service Robot
//
//       Author: M. Marino
//        $Date: 07/08/2013 12:00:00 $
//    $Revision: 1.0 $
//
//  Description:
//
//     Required: libraries:
//     Switches: -
//
//-------------------------------[ History ]-----------------------------------
//
// $Log: zhelpers.h,v $
// Revision 1.0  097/08/2013 12:00:00  M. Marino
// Initial Revision
//
//-----------------------------------------------------------------------------
// (c) Copyright 2013 by MT Robot AG
//*****************************************************************************
///////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif

#include <zmq.h>
#include <czmq.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <dlfcn.h>

#ifdef __cplusplus
}
#endif

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <istream>
#include <ostream>
#include <fstream>
#include <streambuf>
#include <vector>

#define UNUSED(x) (void)(x)

using namespace std;

////////////////////////////////////////////////////////////////////////////////

//  Version checking, and patch up missing constants to match 2.1
#if ZMQ_VERSION_MAJOR == 2
#   error "Please upgrade to ZeroMQ/3.2 for these examples"
#endif

// Define a namespace for this library
namespace mtr_zipc {

//  Provide random number from 0..(num-1)
#ifdef randof
#undef randof
#endif
#define randof(num)  ( (int) ((float) (num) * random () / (RAND_MAX + 1.0)) )


// The zevent handler function type
typedef void *(ZEVENT_HANDLER) (string sSubscription_Key, void *msg, void *client_data);


extern string   zipc_zmq_version ();
extern void     zipc_generate_random_string (string &sS, size_t uiLength);
extern int      zipc_set_signal_handler (void (*f)(int), struct sigaction *ptSig_Action);
extern void     zipc_clear_stringstream(stringstream &strstrS);
extern string   zipc_ui64tos (uint64_t uiI);
extern uint64_t zipc_stoui64 (string &sS);
extern string   zipc_sGetTimeTag (unsigned long *ulMicroseconds);
extern string   zipc_sNow_microsecs ();
extern void     zipc_console (const char *format, ...);
extern void     zipc_sleep (unsigned int msecs);
extern int      zipc_nano_sleep (double dSleep_Time_Seconds);
extern uint64_t zipc_clock ();
extern uint64_t zipc_clock_2 ();
extern double   zipc_clock_3 ();
extern uint64_t zipc_mins_to_msecs(uint64_t min);
extern int      zipc_set_id (void *socket);
extern void     zipc_dump (void *socket);
extern int      zipc_recv (void *socket, char *buffer_recvd, bool bBlocking_Mode = true);
extern int      zipc_recv_buffer (void *socket, void **buffer_recvd, size_t len, bool bBlocking_Mode = true, bool bAllocate_Buffer = true);
extern int      zipc_recv_zmq_msg (void *socket, void **buffer_recvd, size_t len, bool bBlocking_Mode = true);
extern int      zipc_send (void *socket, char *string);
extern int      zipc_send_buffer (void *socket, void *buffer, size_t len);
extern int      zipc_send_zmq_msg (void *socket, void *buffer, size_t len);
extern int      zipc_sendmore (void *socket, char *string);
extern int      zipc_sendmore_buffer (void *socket, void *buffer, size_t len);

extern int      zipc_init_publisher (void **context, void **socket, string sEndpoint, int iZMQ_sndhwm = 0);
extern int      zipc_init_publisher_to_proxy (void **context, void **socket, string sEndpoint, int iZMQ_sndhwm = 1000);
extern int      zipc_init_subscriber (void **context, void **socket, string sEndpoint, int iZMQ_rcvhwm = 0);
extern int      zipc_init_server (void **context, void **socket, string sEndpoint);
extern int      zipc_init_client (void **context, void **socket, string sEndpoint);
extern void     zipc_close_socket (void *socket1 = 0, void *socket2 = 0);
extern void     zipc_destroy_context (void **context);

extern ostream& zipc_serialize_char_array (ostream& out, const char cArray[], size_t len);
extern istream& zipc_deserialize_char_array (istream& in, char cArray[]);
extern ostream& zipc_serialize_string (ostream& out, const string& sString);
extern istream& zipc_deserialize_string (istream& in, string& sString);

extern void *zipc_zevent_handler_test (string, void *, void *);

extern int zipc_iLoad_configuration (string  sFile_Pathname, string &sConfiguration);

////////////////////////////////////////////////////////////////////////////////
}  // end of namespace mtr_zipc

#endif  // _ZHELPERS_H_

// EOF zhelpers.h
