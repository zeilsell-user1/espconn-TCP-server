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

        WHEN("when a receive message is passed from the ESPCONN firmware")
        {
            REQUIRE_EQ(tcpServer.startTcpServer(port, connectedCb, &dummyObject), true);
            setupTest();
            connectCb_(&mockedEspconn);

            char testMessage[] = "This is a test message";
            recvCb_(&mockedEspconn, testMessage, strlen((const char *)testMessage));
            REQUIRE_EQ(receivedCbCalled, true);
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
