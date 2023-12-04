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

        REQUIRE_EQ(tcpServer.startTcpServer(port, connectedCb, &dummyObject), true);

        WHEN("when the connected callback is called successfully")
        {
            setupTest();
            connectCb_(&mockedEspconn);

            REQUIRE_EQ(espconnRegistConnectCbCalled, false);
            REQUIRE_EQ(espconnRegistRecvCbCalled, true);
            REQUIRE_EQ(espconnRegistSentCbCalled, true);
            REQUIRE_EQ(espconnRegistDisconnectCbCalled, true);
            REQUIRE_EQ(espconnRegistReconnectCbCalled, true);

            REQUIRE_EQ(connectedCbCalled, true);
            REQUIRE_EQ(receivedCbCalled, false);
            REQUIRE_EQ(sentCbCalled, false);
            REQUIRE_EQ(disconnectedCbCalled, false);
            REQUIRE_EQ(reconnectCbCalled, false);

            REQUIRE_EQ(espconnAcceptCalled, false);
            REQUIRE_EQ(espconnConnectCalled, false);
            REQUIRE_EQ(espconnSendCalled, false);
            REQUIRE_EQ(espconnDisconnectCalled, false);
            REQUIRE_EQ(espconnAbortTestCalled, false);
        }
        WHEN("when the connected callback is called successfully but received CB reg fails")
        {
            setupTest();
            espconnRegistRecvCbTestIndex = 1;
            espconnAbortTestIndex = 0;

            connectCb_(&mockedEspconn);

            REQUIRE_EQ(espconnRegistConnectCbCalled, false);
            REQUIRE_EQ(espconnRegistRecvCbCalled, true);
            REQUIRE_EQ(espconnRegistSentCbCalled, false);
            REQUIRE_EQ(espconnRegistDisconnectCbCalled, false);
            REQUIRE_EQ(espconnRegistReconnectCbCalled, false);

            REQUIRE_EQ(connectedCbCalled, true);
            REQUIRE_EQ(receivedCbCalled, false);
            REQUIRE_EQ(sentCbCalled, false);
            REQUIRE_EQ(disconnectedCbCalled, false);
            REQUIRE_EQ(reconnectCbCalled, false);

            REQUIRE_EQ(espconnAcceptCalled, false);
            REQUIRE_EQ(espconnConnectCalled, false);
            REQUIRE_EQ(espconnSendCalled, false);
            REQUIRE_EQ(espconnDisconnectCalled, false);
            REQUIRE_EQ(espconnAbortTestCalled, true);
        }
        WHEN("when the connected callback is called successfully but sent CB reg fails")
        {
            setupTest();
            espconnRegistSentCbTestIndex = 1;
            espconnAbortTestIndex = 0;

            connectCb_(&mockedEspconn);

            REQUIRE_EQ(espconnRegistConnectCbCalled, false);
            REQUIRE_EQ(espconnRegistRecvCbCalled, true);
            REQUIRE_EQ(espconnRegistSentCbCalled, true);
            REQUIRE_EQ(espconnRegistDisconnectCbCalled, false);
            REQUIRE_EQ(espconnRegistReconnectCbCalled, false);

            REQUIRE_EQ(connectedCbCalled, true);
            REQUIRE_EQ(receivedCbCalled, false);
            REQUIRE_EQ(sentCbCalled, false);
            REQUIRE_EQ(disconnectedCbCalled, false);
            REQUIRE_EQ(reconnectCbCalled, false);

            REQUIRE_EQ(espconnAcceptCalled, false);
            REQUIRE_EQ(espconnConnectCalled, false);
            REQUIRE_EQ(espconnSendCalled, false);
            REQUIRE_EQ(espconnDisconnectCalled, false);
            REQUIRE_EQ(espconnAbortTestCalled, true);
        }
        WHEN("when the connected callback is called successfully but disconnected CB reg fails")
        {
            setupTest();
            espconnRegistDisconnectCbTestIndex = 1;
            espconnAbortTestIndex = 0;

            connectCb_(&mockedEspconn);

            REQUIRE_EQ(espconnRegistConnectCbCalled, false);
            REQUIRE_EQ(espconnRegistRecvCbCalled, true);
            REQUIRE_EQ(espconnRegistSentCbCalled, true);
            REQUIRE_EQ(espconnRegistDisconnectCbCalled, true);
            REQUIRE_EQ(espconnRegistReconnectCbCalled, false);

            REQUIRE_EQ(connectedCbCalled, true);
            REQUIRE_EQ(receivedCbCalled, false);
            REQUIRE_EQ(sentCbCalled, false);
            REQUIRE_EQ(disconnectedCbCalled, false);
            REQUIRE_EQ(reconnectCbCalled, false);

            REQUIRE_EQ(espconnAcceptCalled, false);
            REQUIRE_EQ(espconnConnectCalled, false);
            REQUIRE_EQ(espconnSendCalled, false);
            REQUIRE_EQ(espconnDisconnectCalled, false);
            REQUIRE_EQ(espconnAbortTestCalled, true);
        }
        WHEN("when the connected callback is called successfully but reconnected CB reg fails")
        {
            setupTest();
            espconnRegistReconnectCbTestIndex = 1;
            espconnAbortTestIndex = 0;

            connectCb_(&mockedEspconn);

            REQUIRE_EQ(espconnRegistConnectCbCalled, false);
            REQUIRE_EQ(espconnRegistRecvCbCalled, true);
            REQUIRE_EQ(espconnRegistSentCbCalled, true);
            REQUIRE_EQ(espconnRegistDisconnectCbCalled, true);
            REQUIRE_EQ(espconnRegistReconnectCbCalled, true);

            REQUIRE_EQ(connectedCbCalled, true);
            REQUIRE_EQ(receivedCbCalled, false);
            REQUIRE_EQ(sentCbCalled, false);
            REQUIRE_EQ(disconnectedCbCalled, false);
            REQUIRE_EQ(reconnectCbCalled, false);

            REQUIRE_EQ(espconnAcceptCalled, false);
            REQUIRE_EQ(espconnConnectCalled, false);
            REQUIRE_EQ(espconnSendCalled, false);
            REQUIRE_EQ(espconnDisconnectCalled, false);
            REQUIRE_EQ(espconnAbortTestCalled, true);
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
