/*******************************************************************************
 * Copyright (c) 2023 George Consultants Ltd.
 * richard.john.george.3@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the “Software”),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *******************************************************************************/

#include <string>
#include "tcp_session.h"
#include "tcp_server.h"

/*******************************************************************************
 * local null callback used to make sure any bugs are handled correctly. This
 * is outside of the class so that the callbacks can work across classes
 *******************************************************************************/

void nullCallback1(void *obj, TcpSession::TcpSessionPtr session)
{
    TCP_ERROR("callback1 called without initialisation");
}
void nullcallback2(void *obj, char *pdata, unsigned short len, TcpSession::TcpSessionPtr session)
{
    TCP_ERROR("callback2 called without initialisation");
}
void nullcallback3(void *obj, signed char err, TcpSession::TcpSessionPtr session)
{
    TCP_ERROR("callback3 called without initialisation");
}

/*
 ******************************************************************************
 * Callback functions to receive data and events from ESPCONN library. These
 * are outside the class so that the ESPCONN library remains indifferent to
 * the details of the client calling it.
 ******************************************************************************
 */

void localSessionDisconnectCb(void *arg)
{
    struct espconn *conn = (struct espconn *)arg;
    const ip_addr_t ipAddress = TcpSession::convertIpAddress(conn->proto.tcp->remote_ip);
    TcpSession::SessionId sessionId = TcpSession::createUniqueIdentifier(ipAddress, conn->proto.tcp->remote_port);

    TcpServer &tcpServer = TcpServer::getInstance();
    TcpSession::TcpSessionPtr tcpSession = tcpServer.getSession(sessionId);
    tcpSession->sessionDisconnected(conn);
}

void localSessionReconnectCb(void *arg, signed char err)
{
    struct espconn *conn = (struct espconn *)arg;
    const ip_addr_t ipAddress = TcpSession::convertIpAddress(conn->proto.tcp->remote_ip);
    TcpSession::SessionId sessionId = TcpSession::createUniqueIdentifier(ipAddress, conn->proto.tcp->remote_port);

    TcpServer &tcpServer = TcpServer::getInstance();
    TcpSession::TcpSessionPtr tcpSession = tcpServer.getSession(sessionId);
    tcpSession->sessionReconnect(conn, err);
}

void localIncomingMessageCb(void *arg, char *pdata, unsigned short len)
{
    struct espconn *conn = (struct espconn *)arg;
    const ip_addr_t ipAddress = TcpSession::convertIpAddress(conn->proto.tcp->remote_ip);
    TcpSession::SessionId sessionId = TcpSession::createUniqueIdentifier(ipAddress, conn->proto.tcp->remote_port);

    TcpServer &tcpServer = TcpServer::getInstance();
    TcpSession::TcpSessionPtr tcpSession = tcpServer.getSession(sessionId);
    tcpSession->sessionIncomingMessage(conn, pdata, len);
}

void localMessageSentCb(void *arg)
{
    struct espconn *conn = (struct espconn *)arg;
    const ip_addr_t ipAddress = TcpSession::convertIpAddress(conn->proto.tcp->remote_ip);
    TcpSession::SessionId sessionId = TcpSession::createUniqueIdentifier(ipAddress, conn->proto.tcp->remote_port);

    TcpServer &tcpServer = TcpServer::getInstance();
    TcpSession::TcpSessionPtr tcpSession = tcpServer.getSession(sessionId);
    
    tcpSession->sessionMessageSent(conn);
}

/*******************************************************************************
 * Class Implemenation - public
 *******************************************************************************/

TcpSession::TcpSession(ip_addr_t ipAddress, unsigned short port, espconn *conn)
    : sessionValid_(true),
      sessionId_(createUniqueIdentifier(ipAddress, port)),
      sessionState_(ESPCONN_CONNECT),
      sessionDisconnectedCbListener_(nullptr),
      incomingMessageCbListener_(nullptr),
      messageSentCbListener_(nullptr),
      sessionDeadCbListener_(nullptr),
      disconnectedCb_(nullCallback1),
      reconnectCb_(nullcallback3),
      incomingMessageCb_(nullcallback2),
      messageSentCb_(nullCallback1),
      deadCb_(nullCallback1)
{
    ip4_addr_copy(sessionConfig_.remote_ip, ipAddress);
    sessionConfig_.remote_port = port;
    sessionConfig_.sessionExpiryIntervalTimeout = 0;
    memcpy(&(serverConn_), conn, sizeof(espconn));
}

TcpSession::~TcpSession()
{
    TCP_INFO("destructor called for session %d", sessionId_);
}

uintptr_t TcpSession::getSessionId()
{
    return sessionId_;
}

bool TcpSession::isSessionValid()
{
    return sessionValid_;
}

void TcpSession::disconnectSession()
{
    if (espconn_disconnect(&serverConn_) != 0)
    {
        espconn_abort(&serverConn_);
    }
}

TcpSession::sendResult TcpSession::sendMessage(unsigned char *pData, unsigned short len)
{
    signed char result = 0;

    if ((result = espconn_send(&serverConn_, pData, len)) == 0)
    {
        return SEND_OK;
    }
    else if (result == ESPCONN_MEM)
    {
        TCP_ERROR("out of memory - send failed");
        espconn_abort(&serverConn_);
        return FAILED_ABORTED;
    }
    else if (result == ESPCONN_ARG)
    {
        TCP_ERROR("serverConn cn't be found - send failed");
        espconn_abort(&serverConn_);
        return FAILED_ABORTED;
    }
    else
    {
        TCP_WARNING("failed to send %d", result);
        return RETRY;
    }

    return SEND_OK;
}

// Register the callback listener and the callbacls

bool TcpSession::registerSessionDisconnectedCb(void (*cb)(void *, TcpSessionPtr session), void *obj)
{
    if (espconn_regist_disconcb(&serverConn_, localSessionDisconnectCb) == 0)
    {
        sessionDisconnectedCbListener_ = obj;
        disconnectedCb_ = cb;
        return true;
    }
    else
    {
        TCP_WARNING("failed to register disconnect callback with ESPCONN");
        espconn_abort(&serverConn_); // couldn't add session so abort the TCP session
        sessionValid_ = false;
        return false;
    }
}

bool TcpSession::registerSessionReconnectCb(void (*cb)(void *, signed char err, TcpSessionPtr session), void *obj)
{
    if (espconn_regist_reconcb(&serverConn_, localSessionReconnectCb) == 0)
    {
        sessionReconnectCbListener_ = obj;
        reconnectCb_ = cb;
        return true;
    }
    else
    {
        TCP_WARNING("failed to register reconnect callback with ESPCONN");
        espconn_abort(&serverConn_); // couldn't add session so abort the TCP session
        sessionValid_ = false;
        return false;
    }
}

bool TcpSession::registerIncomingMessageCb(void (*cb)(void *, char *pdata, unsigned short len, TcpSessionPtr session), void *obj)
{
    if (espconn_regist_recvcb(&serverConn_, localIncomingMessageCb) == 0)
    {
        incomingMessageCbListener_ = obj;
        incomingMessageCb_ = cb;
        return true;
    }
    else
    {
        TCP_WARNING("failed to register receive callback with ESPCONN");
        espconn_abort(&serverConn_); // couldn't add session so abort the TCP session
        sessionValid_ = false;
        return false;
    }
}

bool TcpSession::registerMessageSentCb(void (*cb)(void *, TcpSessionPtr session), void *obj)
{
    if (espconn_regist_sentcb(&serverConn_, localMessageSentCb) == 0)
    {
        messageSentCbListener_ = obj;
        messageSentCb_ = cb;
        return true;
    }
    else
    {
        TCP_WARNING("failed to register sent callback with ESPCONN");
        espconn_abort(&serverConn_); // couldn't add session so abort the TCP session
        sessionValid_ = false;
        return false;
    }
}

void TcpSession::registerSessionDeadCb(void (*cb)(void *obj, TcpSessionPtr session), void *obj)
{
    sessionDeadCbListener_ = obj;
    deadCb_ = cb;
}

// the methods that handle the callbacks.

void TcpSession::sessionDisconnected(espconn *conn)
{
    disconnectedCb_(sessionDisconnectedCbListener_, shared_from_this());
    deadCb_(sessionDeadCbListener_, shared_from_this()); // ATM assume dead follow disconnected immediately
}

void TcpSession::sessionReconnect(espconn *conn, signed char err)
{
    reconnectCb_(sessionDisconnectedCbListener_, err, shared_from_this());
}

void TcpSession::sessionIncomingMessage(espconn *conn, char *pdata, unsigned short length)
{
    incomingMessageCb_(incomingMessageCbListener_, pdata, length, shared_from_this());
}

    int testvar = 1;
void TcpSession::sessionMessageSent(espconn *conn)
{
    try
    {
        messageSentCb_(messageSentCbListener_, shared_from_this());
    }
    catch (const std::exception &e)
    {
        // Catch any standard C++ exceptions
        TCP_ERROR("Exception in sessionMessageSent: %s", e.what());
    }
    catch (...)
    {
        // Catch any other unexpected exceptions
        TCP_ERROR("Unknown exception in sessionMessageSent");
    }
}

// stiatic utility methods

ip_addr_t TcpSession::convertIpAddress(unsigned char *ipAddr)
{
    ip_addr_t ipAddress;
    IP4_ADDR(&ipAddress, ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
    return ipAddress;
}

/*******************************************************************************
 * Class Implemenation - private
 *******************************************************************************/

TcpSession::SessionId TcpSession::createUniqueIdentifier(const ip_addr_t &ipAddress, int port)
{
    // Convert IP address to string
    std::string ipString = std::to_string(ipAddress.addr);

    // Concatenate IP address and port as a string
    std::string combinedString = std::string(ipString) + ":" + std::to_string(port);

    // Hash the combined string to produce a unique identifier
    std::hash<std::string> hasher;
    return hasher(combinedString);
}