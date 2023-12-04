#include <functional>
#include <thread>
#include <doctest.h>
#include <string.h>

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

        WHEN("when a send is passed to the ESPCONN firmware")
        {
            REQUIRE_EQ(tcpServer.startTcpServer(port, connectedCb, &dummyObject), true);
            setupTest();
            connectCb_(&mockedEspconn);
            unsigned char testMessage[] = "This is a test message";
            testSession->sendMessage(testMessage, strlen((const char *)testMessage));
            REQUIRE_EQ(espconnSendCalled, true);

            std::this_thread::sleep_for(std::chrono::seconds(2));

            sentCb_((espconn *)&testEspconn);
            REQUIRE_EQ(sentCbCalled, true);
        }
        WHEN("when a send is passed to the ESPCONN firmware and fails with an arg error")
        {
            REQUIRE_EQ(tcpServer.startTcpServer(port, connectedCb, &dummyObject), true);
            setupTest();
            espconnSendTestIndex = 1;
            connectCb_(&mockedEspconn);
            unsigned char testMessage[] = "This is a test message";
            TcpSession::sendResult result = testSession->sendMessage(testMessage, strlen((const char *)testMessage));
            REQUIRE_EQ(espconnSendCalled, true);
            REQUIRE_EQ(espconnAbortTestCalled, true);
        }
        WHEN("when a send is passed to the ESPCONN firmware and fails with a mem error")
        {
            REQUIRE_EQ(tcpServer.startTcpServer(port, connectedCb, &dummyObject), true);
            setupTest();
            espconnSendTestIndex = 2;
            connectCb_(&mockedEspconn);
            unsigned char testMessage[] = "This is a test message";
            TcpSession::sendResult result = testSession->sendMessage(testMessage, strlen((const char *)testMessage));
            REQUIRE_EQ(espconnSendCalled, true);
            REQUIRE_EQ(espconnAbortTestCalled, true);
        }
        WHEN("when a send is passed to the ESPCONN firmware and fails with an maxnum error")
        {
            REQUIRE_EQ(tcpServer.startTcpServer(port, connectedCb, &dummyObject), true);
            setupTest();
            espconnSendTestIndex = 3;
            connectCb_(&mockedEspconn);
            unsigned char testMessage[] = "This is a test message";
            TcpSession::sendResult result = testSession->sendMessage(testMessage, strlen((const char *)testMessage));
            REQUIRE_EQ(espconnSendCalled, true);
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
