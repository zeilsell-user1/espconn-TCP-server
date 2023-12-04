
#include <doctest.h>
#include <string.h>

#include "tcp_server.h"

#include "mock_owner_cb_functions.h"
#include "mock_espconn_regist_cb.h"

bool connectedCbCalled = false;
bool disconnectedCbCalled = false;
bool reconnectCbCalled = false;
bool sentCbCalled = false;
bool receivedCbCalled = false;

TcpSession::TcpSessionPtr testSession = nullptr;

DummyObject dummyObject;

void disconnectedCb(void *ownerObj, TcpSession::TcpSessionPtr session)
{
    TCP_INFO("disconnectedCb called");
    disconnectedCbCalled = true;
};

void incomingMessageCb(void *ownerObj, char *pdata, unsigned short len, TcpSession::TcpSessionPtr session)
{
    try
    {
        TcpServer &tcpServer = TcpServer::getInstance();
        TcpSession::TcpSessionPtr tmpSession = tcpServer.getSession(testSession->getSessionId());

        REQUIRE_EQ(ownerObj, (void *)&dummyObject);
        REQUIRE_EQ(session->getSessionId(), testSession->getSessionId());
        REQUIRE_EQ(tcpServer.getSessionCount(), 1);
        REQUIRE_EQ(tmpSession->getSessionId(), session->getSessionId());
        REQUIRE_EQ(tmpSession->isSessionValid(), true);
        REQUIRE_EQ(strcmp((const char *)pdata, "This is a test message"), 0);
        REQUIRE_EQ(len, strlen("This is a test message"));
        sentCbCalled = true;
    }
    catch (const std::exception &e)
    {
        // Catch any standard C++ exceptions
        TCP_ERROR("Exception in sentCb: %s", e.what());
    }
    catch (...)
    {
        // Catch any other unexpected exceptions
        TCP_ERROR("Unknown exception in sentCb");
    }
    receivedCbCalled = true;
};

void sentCb(void *ownerObj, TcpSession::TcpSessionPtr session)
{
    try
    {
        TcpServer &tcpServer = TcpServer::getInstance();
        TcpSession::TcpSessionPtr tmpSession = tcpServer.getSession(testSession->getSessionId());

        REQUIRE_EQ(ownerObj, (void *)&dummyObject);
        REQUIRE_EQ(session->getSessionId(), testSession->getSessionId());
        REQUIRE_EQ(tcpServer.getSessionCount(), 1);
        REQUIRE_EQ(tmpSession->getSessionId(), session->getSessionId());
        REQUIRE_EQ(tmpSession->isSessionValid(), true);

        sentCbCalled = true;
    }
    catch (const std::exception &e)
    {
        // Catch any standard C++ exceptions
        TCP_ERROR("Exception in sentCb: %s", e.what());
    }
    catch (...)
    {
        // Catch any other unexpected exceptions
        TCP_ERROR("Unknown exception in sentCb");
    }
};

void reconnectCb(void *ownerObj, signed char err, TcpSession::TcpSessionPtr session)
{
    reconnectCbCalled = true;
};

void connectedCb(void *ownerObj, TcpSession::TcpSessionPtr session)
{
    TCP_INFO("connectCb mock function called");
    REQUIRE_EQ(ownerObj, (void *)&dummyObject);
    TcpServer &tcpServer = TcpServer::getInstance();
    REQUIRE_EQ(session->isSessionValid(), true);
    REQUIRE_EQ(tcpServer.getSessionCount(), 1);
    testSession = session;

    bool imrc = session->registerIncomingMessageCb(incomingMessageCb, (void *)&dummyObject);
    espconnRegistRecvCbTestIndex != 0 ? REQUIRE_EQ(imrc, false) : REQUIRE_EQ(imrc, true);
    if (imrc)
    {
        bool msrc = session->registerMessageSentCb(sentCb, (void *)&dummyObject);
        espconnRegistSentCbTestIndex != 0 ? REQUIRE_EQ(msrc, false) : REQUIRE_EQ(msrc, true);
        if (msrc)
        {
            bool sdrc = session->registerSessionDisconnectedCb(disconnectedCb, (void *)&dummyObject);
            espconnRegistDisconnectCbTestIndex != 0 ? REQUIRE_EQ(sdrc, false) : REQUIRE_EQ(sdrc, true);
            if (sdrc)
            {
                bool srrc = session->registerSessionReconnectCb(reconnectCb, (void *)&dummyObject);
                espconnRegistReconnectCbTestIndex != 0 ? REQUIRE_EQ(srrc, false) : REQUIRE_EQ(srrc, true);
            }
        }
    }

    connectedCbCalled = true;
};