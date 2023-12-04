
#include <functional>
#include <doctest.h>

#include <tcp_server.h>

#include "ip_addr.h"
#include "ip4_addr.h"
#include "ip.h"
#include "espconn.h"

#include "mock_espconn_regist_cb.h"
#include "mock_espconn_functions.h"

SCENARIO("TCP Server can be instantiated")
{
    TcpServer &tcpServer = TcpServer::getInstance();
    tcpServer.cleanup();  // ensure the server is clean from the start

    GIVEN("that TcpServer is a singleton")
    {
        WHEN("getInstance is called twice")
        {
            TcpServer &tcpServer2 = TcpServer::getInstance();

            THEN("the instance is the same and the session count is zero")
            REQUIRE_EQ(&tcpServer, &tcpServer2);
        }
    }
    GIVEN("that TcpServer has just been instantiated")
    {
        WHEN("getSessionCount() is called")
        {
            THEN("the session count is zero")
            REQUIRE_EQ(tcpServer.getSessionCount(), 0);
        }
    }
}