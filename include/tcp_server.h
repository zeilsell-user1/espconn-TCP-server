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

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <map>
#include <cstdint>
#include <memory>
#include <variant>

#ifdef ESP8266
#include <lwip/ip.h>
#include "espconn.h"
#else
#include "../test/ip_addr.h"
#include "../test/ip4_addr.h"
#include "../test/ip.h"
#include "../test/espconn.h"
#endif

#include "defaults.h"
#include "tcp_session.h"

// A TCP (Transmission Control Protocol) server is a network service that
// operates on the transport layer of the Internet Protocol (IP) suite.
// Its primary function is to listen for incoming TCP connections from
// multiple clients and respond to their requests. Additionally, a TCP
// server may initiate outgoing connections to other servers, allowing it
// to act not only as a responder to client requests but also as an initiator
// of communication.
//
// The TCP protocol is implemented in the ESP8266 (ESP01) device. This
// class represents the server within the MQTT server application.

class TcpServer
{
public:
    static TcpServer &getInstance();
    void cleanup();

    bool startTcpServer(unsigned short port, void (*cb)(void *, TcpSession::TcpSessionPtr), void *obj);
    bool startTcpClient(ip_addr_t ipAddress, unsigned short port, void (*cb)(void *, TcpSession::TcpSessionPtr), void *obj);
    bool stopTcpServer();
    bool stopTcpClient(ip_addr_t ipAddress);
    void sessionConnected(void *arg);
    std::size_t getSessionCount();
    TcpSession::TcpSessionPtr getSession(TcpSession::SessionId sessionId);
    void sessionDead(TcpSession::TcpSessionPtr);

    // A drawback of using the RAII (Resource Acquisition Is Initialization) principle is that
    // shared_ptr and unique_ptr both need to have access to the constructor and destructor for
    // the class. Unfortunately this means they need to be public, which sucks. Please DO NOT
    // call TcpServer directly, use getInstance() to get a pointer to the instance

    TcpServer();
    ~TcpServer();

private:
    TcpServer(const TcpServer &) = delete;
    TcpServer &operator=(const TcpServer &) = delete;
    TcpServer(TcpServer &&) = delete;
    TcpServer &operator=(TcpServer &&) = delete;

    TcpSession::TcpSessionPtr createTcpSession(ip_addr_t ipAddress, unsigned short port, espconn *conn);
    bool addSession(TcpSession::SessionId sessionId, const TcpSession::TcpSessionPtr tcpSession);
    void removeSession(TcpSession::SessionId sessionId);
    void removeAllSessions();
    TcpSession::TcpSessionPtr getSession(TcpSession::SessionId sessionId) const;

private:
    struct ServerConfig
    {
        unsigned short port_;
        void *ownerObj_; // the upper layer object that owns the callbacks
        void (*connectedCb_)(void *obj, TcpSession::TcpSessionPtr tcpSession_);

        // Default constructor for ServerConfig
        ServerConfig() : port_(0) {}
        ServerConfig(unsigned short port,
                     void *ownerObj,
                     void (*connectedCb)(void *obj, TcpSession::TcpSessionPtr tcpSession))
        {
            port_ = port;
            ownerObj_ = ownerObj;
            connectedCb_ = connectedCb;
        }
    };

    // Define client config structure

    struct Data
    {
        ip_addr_t ipAddress;
        unsigned short port;
        void *ownerObj; // the upper layer object that owns the callbacks
        void (*connectedCb)(void *obj, TcpSession::TcpSessionPtr tcpSession);
    };
    struct ClientConfig
    {
        unsigned char numberOfClients_;
        std::map<unsigned long, Data> clients_;

        ClientConfig() : numberOfClients_(0) {}
        ClientConfig(ip_addr_t ipAddr,
                     unsigned short port,
                     void *ownerObj,
                     void (*connectedCb)(void *obj, TcpSession::TcpSessionPtr tcpSession)) : numberOfClients_(1)
        {
            Data data;
            data.ipAddress = ipAddr;
            data.port = port;
            data.ownerObj = ownerObj;
            data.connectedCb = connectedCb;
            clients_[ipAddr.addr] = data;
        }

        bool addClient(const ip_addr_t &ipAddr,
                       const unsigned short &port,
                       void *ownerObj,
                       void (*connectedCb)(void *obj, TcpSession::TcpSessionPtr tcpSession))
        {
            if (numberOfClients_ < MAX_TCP_CLIENTS)
            {
                // Check if the client is not already in the map
                if (clients_.find(ipAddr.addr) == clients_.end())
                {
                    Data data;
                    data.ipAddress = ipAddr;
                    data.port = port;
                    data.ownerObj = ownerObj;
                    data.connectedCb = connectedCb;
                    clients_[ipAddr.addr] = data;
                    numberOfClients_++;
                    return true; // Successfully added
                }
                else
                {
                    TCP_ERROR("Client already exists.");
                    return false; // Client already exists
                }
            }
            else
            {
                TCP_ERROR("Maximum number of clients reached.");
                return false; // Maximum clients reached
            }
        }

        bool removeClient(const ip_addr_t &ipAddr)
        {
            auto it = clients_.find(ipAddr.addr);
            if (it != clients_.end())
            {
                clients_.erase(it);
                --numberOfClients_;
                return true; // Successfully removed
            }
            else
            {
                TCP_WARNING("Client not found.");
                return false; // Client not found
            }
        }

        void cleanClientList()
        {
            clients_.clear();
            numberOfClients_ = 0;
        }
    };

    // Define the tagged union for server and client configs using std::variant
    using ConfigUnion = std::variant<ServerConfig, ClientConfig>;

private:
    bool started_;
    static std::unique_ptr<TcpServer> instance_;
    ConfigUnion config_; // configuration for either the server or client
    espconn serverConn_; // defines the local server regardless of client or server sessions
    esp_tcp tcpConfig_;  // defines the local server regardless of client or server sessions
    std::map<TcpSession::SessionId, TcpSession::TcpSessionPtr> tcpSessions_;
    void (*sessionDeadCb_)(void *obj, TcpSession::TcpSessionPtr tcpSession_);
};

#endif // TCP_SERVER_H