
#include <functional>
#include <doctest.h>

#include <tcp_server.h>

#include "ip_addr.h"
#include "ip4_addr.h"
#include "ip.h"
#include "espconn.h"

#include "mock_espconn_regist_cb.h"
#include "mock_espconn_functions.h"
#include "mock_owner_cb_functions.h"
#include "mock_utils.h"

SCENARIO("TCP Server being started")
{
    GIVEN("that the TcpServer has been retrieved with a getInstance")
    {
        TcpServer &tcpServer = TcpServer::getInstance();
        unsigned short port = TEST_PORT_1;
        ip_addr_t ipAddress;
        IP4_ADDR(&ipAddress, IP_1, IP_2, IP_3, IP_4);
        espconnRegistConnectCbTestIndex = 0; // successful registration of connect CB
        espconnRegistConnectCbCalled = false;

        WHEN("get a startTcpClient for the first time")
        {
            tcpServer.cleanup(); // ensure the server is clean from the start

            THEN("the result of calling startTcpClient is true")
            {
                REQUIRE_EQ(tcpServer.startTcpClient(ipAddress, port, connectedCb, &dummyObject), true);
                REQUIRE_EQ(espconnRegistConnectCbCalled, true);
            }
        }
        WHEN("startTcpClient is called with same IP address and port")
        {
            THEN("the result of calling startTcpClient with same IP address and port is false")
            {
                REQUIRE_EQ(tcpServer.startTcpClient(ipAddress, port, connectedCb, &dummyObject), false);
                REQUIRE_EQ(espconnRegistConnectCbCalled, false);
            }
        }
        WHEN("startTcpServer is called")
        {
            THEN("the result of calling startTcpServer once cleint is started")
            {
                REQUIRE_EQ(tcpServer.startTcpServer(port, connectedCb, &dummyObject), false);
                REQUIRE_EQ(espconnRegistConnectCbCalled, false);
            }
        }
        WHEN("startTcpClient is called with second IP address and port")
        {
            THEN("the result of calling startTcpClient with second IP address and port is true")
            {
                ipOffset = 1;
                IP4_ADDR(&ipAddress, IP_1, IP_2, IP_3, IP_4 + ipOffset);
                REQUIRE_EQ(tcpServer.startTcpClient(ipAddress, port, connectedCb, &dummyObject), true);
                REQUIRE_EQ(espconnRegistConnectCbCalled, true);
            }
        }
        WHEN("startTcpClient is called with third IP address and port")
        {
            THEN("the result of calling startTcpClient with second IP address and port is true")
            {
                ipOffset = 2;
                IP4_ADDR(&ipAddress, IP_1, IP_2, IP_3, IP_4 + ipOffset);
                REQUIRE_EQ(tcpServer.startTcpClient(ipAddress, port, connectedCb, &dummyObject), true);
                REQUIRE_EQ(espconnRegistConnectCbCalled, true);
            }
        }
        WHEN("startTcpClient is called with third IP address and port")
        {
            THEN("the result of calling startTcpClient with third IP address and port is true")
            {
                ipOffset = 3;
                IP4_ADDR(&ipAddress, IP_1, IP_2, IP_3, IP_4 + ipOffset);
                REQUIRE_EQ(tcpServer.startTcpClient(ipAddress, port, connectedCb, &dummyObject), true);
                REQUIRE_EQ(espconnRegistConnectCbCalled, true);
            }
        }
        WHEN("startTcpClient is called with fourth IP address and port")
        {
            THEN("the result of calling startTcpClient with fourth IP address and port is fail (max tcp clients = 3)")
            {
                ipOffset = 4;
                IP4_ADDR(&ipAddress, IP_1, IP_2, IP_3, IP_4 + ipOffset);
                REQUIRE_EQ(tcpServer.startTcpClient(ipAddress, port, connectedCb, &dummyObject), false);
                REQUIRE_EQ(espconnRegistConnectCbCalled, false);
            }
        }
        WHEN("stopTcpClient is called with third IP address and port")
        {
            THEN("the result of calling stopTcpClient with third IP address and port is true")
            {
                ipOffset = 3;
                IP4_ADDR(&ipAddress, IP_1, IP_2, IP_3, IP_4 + ipOffset);
                REQUIRE_EQ(tcpServer.stopTcpClient(ipAddress), true);
            }
        }
        WHEN("stopTcpClient is called with third IP address and port (already stopped)")
        {
            THEN("the result of calling stopTcpClient with third IP address and port is false")
            {
                ipOffset = 3;
                IP4_ADDR(&ipAddress, IP_1, IP_2, IP_3, IP_4 + ipOffset);
                REQUIRE_EQ(tcpServer.stopTcpClient(ipAddress), false);
            }
        }
        WHEN("startTcpClient is called with fourth IP address and port")
        {
            THEN("the result of calling startTcpClient with fourth IP address and port now works")
            {
                ipOffset = 4;
                IP4_ADDR(&ipAddress, IP_1, IP_2, IP_3, IP_4 + ipOffset);
                REQUIRE_EQ(tcpServer.startTcpClient(ipAddress, port, connectedCb, &dummyObject), true);
                REQUIRE_EQ(espconnRegistConnectCbCalled, true);
            }
        }
        WHEN("error in the registration of the callback")
        {
            tcpServer.cleanup();                 // ensure the server is clean from the start
            espconnRegistConnectCbTestIndex = 1; // unsuccessful registration of connect CB

            THEN("the result of calling startTcpClient is false")
            {
                REQUIRE_EQ(tcpServer.startTcpClient(ipAddress, port, connectedCb, &dummyObject), false);
                REQUIRE_EQ(espconnRegistConnectCbCalled, true);
            }
        }
        WHEN("clean up test")
        {
            tcpServer.cleanup();                 // ensure the server is clean from the start
            THEN("tcpServer should be empty again")
            {
                REQUIRE_EQ(tcpServer.getSessionCount(), 0);
            }
        }
    }
}