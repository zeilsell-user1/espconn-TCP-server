#include <functional>
#include <thread>
#include <string.h>
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

SCENARIO("TCP Server can be started and all other callbacks can be registered when connect is received")
{
    TcpServer &tcpServer = TcpServer::getInstance();
    tcpServer.cleanup();

    GIVEN("that the TcpServer has been retrieved with a getInstance and started as a server")
    {
        espconn mockedEspconn;
        unsigned short port = TEST_PORT_1;

        esp_tcp tcp;
        mockedEspconn.type = ESPCONN_TCP;
        mockedEspconn.state = ESPCONN_CONNECT;
        tcp.remote_ip[0] = IP_1;
        tcp.remote_ip[1] = IP_2;
        tcp.remote_ip[2] = IP_3;
        tcp.remote_ip[3] = IP_4;
        tcp.remote_port = TEST_PORT_1;
        mockedEspconn.proto.tcp = &tcp;

        espconnRegistConnectCbTestIndex = 0;

        WHEN("when a disconnect message is passed to the ESPCONN firmware")
        {
            REQUIRE_EQ(tcpServer.startTcpServer(port, connectedCb, &dummyObject), true);
            setupTest();
            espconnDisconnectTestIndex = 0;
            connectCb_(&mockedEspconn);
            REQUIRE_EQ(tcpServer.getSessionCount(), 1);
            testSession->disconnectSession();
            TCP_INFO("Sleep for 2 seconds");
            std::this_thread::sleep_for(std::chrono::seconds(2));

            TCP_INFO("confirm the disconnect from ESPCONN");
            disconnectedCb_(&testEspconn);
            TCP_INFO("confirm the session is removed");
            REQUIRE_EQ(espconnDisconnectCalled, true);
            REQUIRE_EQ(tcpServer.getSessionCount(), 0);
        }
        WHEN("when a disconnect message is passed to the ESPCONN firmware but gets an error")
        {
            REQUIRE_EQ(tcpServer.startTcpServer(port, connectedCb, &dummyObject), true);
            setupTest();
            espconnDisconnectTestIndex = 1; // fail the ESPCONN response
            connectCb_(&mockedEspconn);
            REQUIRE_EQ(tcpServer.getSessionCount(), 1);
            testSession->disconnectSession();
            REQUIRE_EQ(espconnDisconnectCalled, true);
            REQUIRE_EQ(espconnAbortTestCalled, true);
            tcpServer.sessionDisconnected(testSession->getSessionId());
            REQUIRE_EQ(tcpServer.getSessionCount(), 0);
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
