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

#include "tcp_server.h"

/*******************************************************************************
 * serverConn is required to be valid for the entire life of the TCP server and
 * so is defined here as a global variable
 *******************************************************************************/

/*******************************************************************************
 * A dummy callback used in case the upper layer has not defined a real
 * callback. This makes sure any bugs are handled correctly. This callback
 * is outside of the class so that the callbacks can work across classes
 *******************************************************************************/

void nullCallback(void *obj, TcpSession::TcpSessionPtr tcpSession)
{
    TCP_ERROR("callback called without initialisation");
}

/*******************************************************************************
 * This callback is used by the ESPCONN software when a connect has been
 * received. It is outside the contect of the TcpServer and so uses the
 * getInstance to get the TcpServer object.
 *
 * Currenlty there is no difference bwetween a client session and an server
 * session so they both call the same handle for sessionConnected
 *******************************************************************************/

void espconnSessionConnectedCb(void *arg)
{
    TcpServer &tcpServer = TcpServer::getInstance();
    tcpServer.sessionConnected(arg);
}

/*******************************************************************************
 * Class Implemenation - private
 *******************************************************************************/

TcpServer::TcpServer()
    : started_(false),
      config_{} // Use the default constructor for ServerConfig
{
}

TcpServer::~TcpServer()
{
    tcpSessions_.clear(); // Clear the map
}

/*******************************************************************************
 * Class Implemenation - public
 *******************************************************************************/

std::unique_ptr<TcpServer> TcpServer::instance_ = nullptr;

TcpServer &TcpServer::getInstance()
{
    if (!instance_)
    {
        instance_ = std::make_unique<TcpServer>();
    }
    return *instance_;
}

void TcpServer::cleanup()
{
    started_ = false;
    removeAllSessions();

    if (std::holds_alternative<ClientConfig>(config_))
    {
        auto &clientConfig = std::get<ClientConfig>(config_);
        clientConfig.cleanClientList();
    }
}

bool TcpServer::startTcpServer(unsigned short port,
                               void (*cb)(void *, TcpSession::TcpSessionPtr),
                               void *ownerObj)
{
    if (started_ == true)
    {
        TCP_WARNING("Trying to start the server a second time");
        return false;
    }

    config_ = ServerConfig{port, ownerObj, cb};

    // Set up server configuration
    serverConn_.type = ESPCONN_TCP;
    serverConn_.state = ESPCONN_NONE;
    serverConn_.proto.tcp = &tcpConfig_;
    serverConn_.proto.tcp->local_port = port;

    if (espconn_regist_connectcb(&serverConn_, espconnSessionConnectedCb) != 0)
    {
        cleanup();
        return false;
    }
    // TODO espconn_regist_reconcb(&serverConn, espconnClientSessionReconnectedCb);

    signed char rc = espconn_accept(&serverConn_); // Enable server

    switch (rc)
    {
    case 0:
        started_ = true;
        return true;
    case ESPCONN_MEM:
        TCP_WARNING("espconn_connect returned ESPCONN_MEM");
        cleanup();
        return false;
    case ESPCONN_ISCONN:
        TCP_WARNING("espconn_connect returned MQTT_WARNING");
        cleanup();
        return false;
    case ESPCONN_ARG:
        TCP_WARNING("espconn_connect returned MQTT_WARNING");
        cleanup();
        return false;
    default:
        TCP_WARNING("espconn_connect returned %d", rc);
        cleanup();
        return false;
    }
}

bool TcpServer::startTcpClient(ip_addr_t ipAddress,
                               unsigned short port,
                               void (*cb)(void *,
                                TcpSession::TcpSessionPtr),
                               void *ownerObj)
{

    if ((started_ == true) && (std::holds_alternative<ServerConfig>(config_)))
    {
        TCP_WARNING("Trying to start the client while a server is active");
        return false;
    }
    else if (std::holds_alternative<ClientConfig>(config_))
    {
        auto &clientConfig = std::get<ClientConfig>(config_);

        if (!clientConfig.addClient(ipAddress, port, ownerObj, cb))
        {
            TCP_WARNING("Adding client failed");
            return false;
        }
    }
    else
    {
        config_ = ClientConfig{ipAddress, port, ownerObj, cb};
    }

    // Set the server's port
    serverConn_.type = ESPCONN_TCP;
    serverConn_.state = ESPCONN_NONE;
    serverConn_.proto.tcp = &tcpConfig_;
    serverConn_.proto.tcp->remote_ip[0] = ip4_addr1(&ipAddress);
    serverConn_.proto.tcp->remote_ip[1] = ip4_addr2(&ipAddress);
    serverConn_.proto.tcp->remote_ip[2] = ip4_addr3(&ipAddress);
    serverConn_.proto.tcp->remote_ip[3] = ip4_addr4(&ipAddress);
    serverConn_.proto.tcp->remote_port = port;

    if (espconn_regist_connectcb(&serverConn_, espconnSessionConnectedCb) != 0)
    {
        cleanup();
        return false;
    }

    // TODO espconn_regist_reconcb(&serverConn, espconnClientSessionReconnectedCb);

    signed char rc = espconn_connect(&serverConn_);

    switch (rc)
    {
    case 0:
        started_ = true;
        return true;
    case ESPCONN_RTE:
        TCP_WARNING("espconn_connect returned ESPCONN_RTE");
        cleanup();
        return false;
    case ESPCONN_MEM:
        TCP_WARNING("espconn_connect returned ESPCONN_MEM");
        cleanup();
        return false;
    case ESPCONN_ISCONN:
        TCP_WARNING("espconn_connect returned MQTT_WARNING");
        cleanup();
        return false;
    case ESPCONN_ARG:
        TCP_WARNING("espconn_connect returned MQTT_WARNING");
        cleanup();
        return false;
    default:
        TCP_WARNING("espconn_connect returned %d", rc);
        cleanup();
        return false;
    }
}
bool TcpServer::stopTcpServer()
{
    // TODO
    return true;
}

bool TcpServer::stopTcpClient(ip_addr_t ipAddress)
{
    if ((started_ == true) && (std::holds_alternative<ServerConfig>(config_)))
    {
        TCP_WARNING("Trying to stop the client while a server is active");
        return false;
    }
    else if (std::holds_alternative<ClientConfig>(config_))
    {
        auto &clientConfig = std::get<ClientConfig>(config_);

        if (clientConfig.removeClient(ipAddress))
        {
            return true;
        }
    }
    else
    {
        TCP_WARNING("Trying to stop the client that doesn't exist");
        return false;
    }

    return false;
}

/*******************************************************************************
 * methods to handle the espconn events
 *******************************************************************************/

// Server connection callback. The ESPCONN has returned a session that is pointed
// to by the espconn struct in arg. The contents of this struct are transfered
// to a TCP session to abstract ESPCONN from the rest of the system. The TCP
// session is then passed to the owning object via the already-registered
// callback.

void TcpServer::sessionConnected(void *arg)
{
    struct espconn *conn = (struct espconn *)arg;

    if ((conn->type == ESPCONN_TCP) && (conn->state == ESPCONN_CONNECT))
    {
        TcpSession::TcpSessionPtr tcpSessionPtr =
            createTcpSession(TcpSession::convertIpAddress(conn->proto.tcp->remote_ip),
                             (unsigned short)conn->proto.tcp->remote_port,
                             conn);
    }
}

void TcpServer::sessionDisconnected(TcpSession::SessionId sessionId)
{
    removeSession(sessionId);
}

TcpSession::TcpSessionPtr TcpServer::createTcpSession(ip_addr_t ipAddress, unsigned short port, espconn *conn)
{
    try
    {
        auto tcpSessionPtr = std::make_shared<TcpSession>(TcpSession::convertIpAddress(conn->proto.tcp->remote_ip),
                                                          (unsigned short)conn->proto.tcp->remote_port,
                                                          conn);

        TcpSession::SessionId sessionId = tcpSessionPtr->getSessionId();

        if (!addSession(sessionId, tcpSessionPtr))
        {
            espconn_abort(conn); // couldn't add session so abort the TCP session
        }
        else // call the owner and let them know a session is active
        {
            if (std::holds_alternative<ServerConfig>(config_))
            {
                auto &serverConfig = std::get<ServerConfig>(config_);
                serverConfig.connectedCb_(serverConfig.ownerObj_, tcpSessionPtr);
            }
            else // client
            {
                auto &clientConfig = std::get<ClientConfig>(config_);

                auto it = clientConfig.clients_.find(ipAddress.addr);
                if (it != clientConfig.clients_.end())
                {
                    TCP_WARNING("Client not found.");
                }
                else // Found the client
                {
                    const Data& clientData = it->second;
                    clientData.connectedCb(clientData.ownerObj, tcpSessionPtr);
                }
            }
        }
        return tcpSessionPtr;
    }
    catch (const std::exception &e)
    {
        TCP_ERROR("failed to create a session");
        espconn_abort(conn);
        return nullptr;
    }
}

bool TcpServer::addSession(TcpSession::SessionId sessionId, const TcpSession::TcpSessionPtr session)
{
    // Check if the identifier is already in use
    if (tcpSessions_.find(sessionId) == tcpSessions_.end())
    {
        if (tcpSessions_.size() < MAX_TCP_SESSIONS)
        {
            tcpSessions_[sessionId] = session;
            return true;
        }
        else
        {
            TCP_WARNING("too many sessions");
            return false;
        }
    }

    TCP_WARNING("session already in the array");
    return true;
}

void TcpServer::removeSession(TcpSession::SessionId sessionId)
{
    auto it = tcpSessions_.find(sessionId);
    if (it != tcpSessions_.end())
    {
        tcpSessions_.erase(it);
    }
    else
    {
        TCP_WARNING("session not found in the map");
    }
}

std::size_t TcpServer::getSessionCount()
{
    return tcpSessions_.size();
}

TcpSession::TcpSessionPtr TcpServer::getSession(TcpSession::SessionId sessionId)
{
    auto it = tcpSessions_.find(sessionId);
    if (it != tcpSessions_.end())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void TcpServer::removeAllSessions()
{
    // Clear the map, releasing all shared_ptr and deallocating memory
    tcpSessions_.clear();
}

// espconn_regist_recvcb(&serverConn, serverRecvCb);
// espconn_regist_disconcb(&serverConn, serverDisconCb);