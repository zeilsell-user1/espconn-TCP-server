
#ifndef __MOCK_ESPCONN_REGIST_CB_H__
#define __MOCK_ESPCONN_REGIST_CB_H__

#include <stdbool.h>
#include "espconn.h"

extern int espconnRegistConnectCbTestIndex;
extern int espconnRegistDisconnectCbTestIndex;
extern int espconnRegistRecvCbTestIndex;
extern int espconnRegistSentCbTestIndex;
extern int espconnRegistReconnectCbTestIndex;

extern bool espconnRegistConnectCbCalled;
extern bool espconnRegistDisconnectCbCalled;
extern bool espconnRegistRecvCbCalled;
extern bool espconnRegistSentCbCalled;
extern bool espconnRegistReconnectCbCalled;

extern espconn_connect_callback connectCb_;
extern espconn_sent_callback sentCb_;
extern espconn_recv_callback recvCb_;
extern espconn_connect_callback disconnectedCb_;
extern espconn_reconnect_callback reconnectCb_;

#endif // __MOCK_ESPCONN_REGIST_CB_H__