
#ifndef __MOCK_OWNER_CB_FUNCTIONS_H__
#define __MOCK_OWNER_CB_FUNCTIONS_H__

#include "tcp_session.h"

extern bool connectedCbCalled;
extern bool disconnectedCbCalled;
extern bool reconnectCbCalled;
extern bool sentCbCalled;
extern bool receivedCbCalled;
extern TcpSession::TcpSessionPtr testSession;

class DummyObject
{
};

extern DummyObject dummyObject;

void disconnectedCb(void *ownerObj, TcpSession::TcpSessionPtr session);
void incomingMessageCb(void *ownerObj, char *pdata, unsigned short len, TcpSession::TcpSessionPtr session);
void sentCb(void *ownerObj, TcpSession::TcpSessionPtr session);
void reconnectCb(void *ownerObj, signed char err, TcpSession::TcpSessionPtr session);
void connectedCb(void *ownerObj, TcpSession::TcpSessionPtr session);

#endif // __MOCK_ESPCONN_FUNCTIONS_H__
