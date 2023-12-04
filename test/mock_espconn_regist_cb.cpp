
#include <doctest.h>

#include "ip_addr.h"
#include "ip4_addr.h"
#include "ip.h"
#include "espconn.h"

#include "mock_espconn_regist_cb.h"

int espconnRegistConnectCbTestIndex = 0;
int espconnRegistDisconnectCbTestIndex = 0;
int espconnRegistRecvCbTestIndex = 0;
int espconnRegistSentCbTestIndex = 0;
int espconnRegistReconnectCbTestIndex = 0;

bool espconnRegistConnectCbCalled = false;
bool espconnRegistDisconnectCbCalled = false;
bool espconnRegistRecvCbCalled = false;
bool espconnRegistSentCbCalled = false;
bool espconnRegistReconnectCbCalled = false;

espconn_connect_callback connectCb_;
espconn_sent_callback sentCb_;
espconn_recv_callback recvCb_;
espconn_connect_callback disconnectCb_;
espconn_reconnect_callback reconnectCb_;

// the callback to the UUT

signed char espconn_regist_connectcb(struct espconn *espconn, espconn_connect_callback connCb)
{
    INFO("This mock is called when the TCP Server registers a connect CB with the ESPCONN library");

    espconnRegistConnectCbCalled = true;

    if (espconnRegistConnectCbTestIndex == 0)
    {
        INFO("Successful registration of connect CB");
        REQUIRE_EQ(espconn->proto.tcp->local_port, TEST_PORT_1);
        connectCb_ = connCb;
        return 0;
    }
    else if (espconnRegistConnectCbTestIndex == 1)
    {
        INFO("return a generic arg issue to espconn connect");
        return ESPCONN_ARG;
    }
    else
    {
        FAIL("Not yet developed - espconnAbortTestIndex out of range");
        return ESPCONN_ARG;
    }

    return 0;
}

signed char espconn_regist_sentcb(struct espconn *espconn, espconn_sent_callback sentCb)
{
    INFO("This mock is called when the TCP Server registers a sent CB with the ESPCONN library");

    espconnRegistSentCbCalled = true;

    if (espconnRegistSentCbTestIndex == 0)
    {
        INFO("Successful registeration of sent CB");
        ip_addr_t ipAddressPassed;
        IP4_ADDR(&ipAddressPassed, espconn->proto.tcp->remote_ip[0],
                 espconn->proto.tcp->remote_ip[1],
                 espconn->proto.tcp->remote_ip[2],
                 espconn->proto.tcp->remote_ip[3]);
        ip_addr_t ipAddressTest;
        IP4_ADDR(&ipAddressTest, IP_1, IP_2, IP_3, IP_4);
        REQUIRE_EQ(ipAddressPassed.addr, ipAddressTest.addr);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        sentCb_ = sentCb;
    }
    else if (espconnRegistSentCbTestIndex == 1)
    {
        INFO("return a generic arg issue to espconn sent");
        return ESPCONN_ARG;
    }
    else
    {
        FAIL("Not yet developed - espconnAbortTestIndex out of range");
        return ESPCONN_ARG;
    }

    return 0;
}

signed char espconn_regist_recvcb(struct espconn *espconn, espconn_recv_callback recvCb)
{
    INFO("This mock is called when the TCP Server registers a receive CB with the ESPCONN library");

    espconnRegistRecvCbCalled = true;

    if (espconnRegistRecvCbTestIndex == 0)
    {
        INFO("Successful registeration of receive CB");
        ip_addr_t ipAddressPassed;
        IP4_ADDR(&ipAddressPassed, espconn->proto.tcp->remote_ip[0],
                 espconn->proto.tcp->remote_ip[1],
                 espconn->proto.tcp->remote_ip[2],
                 espconn->proto.tcp->remote_ip[3]);
        ip_addr_t ipAddressTest;
        IP4_ADDR(&ipAddressTest, IP_1, IP_2, IP_3, IP_4);
        REQUIRE_EQ(ipAddressPassed.addr, ipAddressTest.addr);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        recvCb_ = recvCb;
    }
    else if (espconnRegistRecvCbTestIndex == 1)
    {
        INFO("return a generic arg issue to espconn receive");
        return ESPCONN_ARG;
    }
    else
    {
        FAIL("Not yet developed - espconnAbortTestIndex out of range");
        return ESPCONN_ARG;
    }

    return 0;
}

signed char espconn_regist_reconcb(struct espconn *espconn, espconn_reconnect_callback reconCb)
{
    INFO("This mock is called when the TCP Server registers a reconnect CB with the ESPCONN library");

    espconnRegistReconnectCbCalled = true;

    if (espconnRegistReconnectCbTestIndex == 0)
    {
        INFO("Successful registeration of reconnect CB");
        ip_addr_t ipAddressPassed;
        IP4_ADDR(&ipAddressPassed, espconn->proto.tcp->remote_ip[0],
                 espconn->proto.tcp->remote_ip[1],
                 espconn->proto.tcp->remote_ip[2],
                 espconn->proto.tcp->remote_ip[3]);
        ip_addr_t ipAddressTest;
        IP4_ADDR(&ipAddressTest, IP_1, IP_2, IP_3, IP_4);
        REQUIRE_EQ(ipAddressPassed.addr, ipAddressTest.addr);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        reconnectCb_ = reconCb;
    }
    else if (espconnRegistReconnectCbTestIndex == 1)
    {
        INFO("return a generic arg issue to espconn reconnect");
        return ESPCONN_ARG;
    }
    else
    {
        FAIL("Not yet developed - espconnAbortTestIndex out of range");
        return ESPCONN_ARG;
    }

    return 0;
}

signed char espconn_regist_disconcb(struct espconn *espconn, espconn_connect_callback disconCb)
{
    INFO("This mock is called when the TCP Server registers s disconnect CB with the ESPCONN library");

    espconnRegistDisconnectCbCalled = true;

    if (espconnRegistDisconnectCbTestIndex == 0)
    {
        INFO("Successful registeration of disconnect CB");
        ip_addr_t ipAddressPassed;
        IP4_ADDR(&ipAddressPassed, espconn->proto.tcp->remote_ip[0],
                 espconn->proto.tcp->remote_ip[1],
                 espconn->proto.tcp->remote_ip[2],
                 espconn->proto.tcp->remote_ip[3]);
        ip_addr_t ipAddressTest;
        IP4_ADDR(&ipAddressTest, IP_1, IP_2, IP_3, IP_4);
        REQUIRE_EQ(ipAddressPassed.addr, ipAddressTest.addr);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        disconnectCb_ = disconCb;
    }
    else if (espconnRegistDisconnectCbTestIndex == 1)
    {
        INFO("return a generic arg issue to espconn register disconnect");
        return ESPCONN_ARG;
    }
    else
    {
        FAIL("Not yet developed - espconnAbortTestIndex out of range");
        return ESPCONN_ARG;
    }

    return 0;
}