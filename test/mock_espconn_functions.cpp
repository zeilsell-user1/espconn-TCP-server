
#include <doctest.h>
#include <string.h>

#include "ip_addr.h"
#include "ip4_addr.h"
#include "ip.h"
#include "espconn.h"

#include "mock_espconn_functions.h"
#include "mock_espconn_regist_cb.h"
#include "mock_owner_cb_functions.h"

int espconnAcceptTestIndex = 0;
int espconnConnectTestIndex = 0;
int espconnSendTestIndex = 0;
int espconnDisconnectTestIndex = 0;
int espconnAbortTestIndex = 99;

bool espconnAcceptCalled = false;
bool espconnConnectCalled = false;
bool espconnSendCalled = false;
bool espconnDisconnectCalled = false;
bool espconnAbortTestCalled = false;

unsigned char ipOffset = 0;

struct espconn testEspconn;

// the mocked calls from UUT to ESPCONN

signed char espconn_accept(struct espconn *espconn)
{
    INFO("This mock is called when the TCP Server configures the ESPCONN library as a listener");

    espconnAcceptCalled = true;

    if (espconnAcceptTestIndex == 0) // a successful return
    {
        INFO("Successful accept");
        REQUIRE_EQ(espconn->proto.tcp->local_port, TEST_PORT_1);
        return 0;
    }
    else if (espconnAcceptTestIndex == 1)
    {
        FAIL("Returns a fail for out of memory");
        REQUIRE_EQ(espconn->proto.tcp->local_port, TEST_PORT_1);
        return ESPCONN_MEM;
    }
    else if (espconnAcceptTestIndex == 2)
    {
        FAIL("Returns a fail for already connected");
        REQUIRE_EQ(espconn->proto.tcp->local_port, TEST_PORT_1);
        return ESPCONN_ISCONN;
    }
    else if (espconnAcceptTestIndex == 3)
    {
        FAIL("Returns a fail for incorrect arguement");
        return ESPCONN_ARG;
    }
    else
    {
        FAIL("Not yet developed - espconnAcceptTestIndex out of range");
        return ESPCONN_ARG;
    }

    return 0;
}

signed char espconn_connect(struct espconn *espconn)
{
    INFO("This mock is called when the TCP Server connects as a client using the ESPCONN library");

    espconnConnectCalled = true;

    if (espconnConnectTestIndex == 0)
    {
        INFO("successful connection");
        ip_addr_t ipAddressPassed;
        IP4_ADDR(&ipAddressPassed, espconn->proto.tcp->remote_ip[0],
                 espconn->proto.tcp->remote_ip[1],
                 espconn->proto.tcp->remote_ip[2],
                 espconn->proto.tcp->remote_ip[3]);
        ip_addr_t ipAddressTest;
        IP4_ADDR(&ipAddressTest, IP_1, IP_2, IP_3, IP_4 + ipOffset);
        REQUIRE_EQ(ipAddressPassed.addr, ipAddressTest.addr);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        return 0;
    }
    else if (espconnConnectTestIndex == 1)
    {
        FAIL("return a routing problem to the espconn connect");
        ip_addr_t ipAddressPassed;
        IP4_ADDR(&ipAddressPassed, espconn->proto.tcp->remote_ip[0],
                 espconn->proto.tcp->remote_ip[1],
                 espconn->proto.tcp->remote_ip[2],
                 espconn->proto.tcp->remote_ip[3]);
        ip_addr_t ipAddressTest;
        IP4_ADDR(&ipAddressTest, IP_1, IP_2, IP_3, IP_4);
        REQUIRE_EQ(ipAddressPassed.addr, ipAddressTest.addr);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        return ESPCONN_RTE;
    }
    else if (espconnConnectTestIndex == 2)
    {
        FAIL("return an out of memory to the espconn connect");
        ip_addr_t ipAddressPassed;
        IP4_ADDR(&ipAddressPassed, espconn->proto.tcp->remote_ip[0],
                 espconn->proto.tcp->remote_ip[1],
                 espconn->proto.tcp->remote_ip[2],
                 espconn->proto.tcp->remote_ip[3]);
        ip_addr_t ipAddressTest;
        IP4_ADDR(&ipAddressTest, IP_1, IP_2, IP_3, IP_4);
        REQUIRE_EQ(ipAddressPassed.addr, ipAddressTest.addr);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        return ESPCONN_MEM;
    }
    else if (espconnConnectTestIndex == 3)
    {
        FAIL("return an already connect to the espconn connect");
        ip_addr_t ipAddressPassed;
        IP4_ADDR(&ipAddressPassed, espconn->proto.tcp->remote_ip[0],
                 espconn->proto.tcp->remote_ip[1],
                 espconn->proto.tcp->remote_ip[2],
                 espconn->proto.tcp->remote_ip[3]);
        ip_addr_t ipAddressTest;
        IP4_ADDR(&ipAddressTest, IP_1, IP_2, IP_3, IP_4);
        REQUIRE_EQ(ipAddressPassed.addr, ipAddressTest.addr);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        return ESPCONN_ISCONN;
    }
    else if (espconnConnectTestIndex == 4)
    {
        FAIL("return a generic arg issue to espconn connect");
        ip_addr_t ipAddressPassed;
        IP4_ADDR(&ipAddressPassed, espconn->proto.tcp->remote_ip[0],
                 espconn->proto.tcp->remote_ip[1],
                 espconn->proto.tcp->remote_ip[2],
                 espconn->proto.tcp->remote_ip[3]);
        ip_addr_t ipAddressTest;
        IP4_ADDR(&ipAddressTest, IP_1, IP_2, IP_3, IP_4);
        REQUIRE_EQ(ipAddressPassed.addr, ipAddressTest.addr);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        return ESPCONN_ARG;
    }
    else
    {
        FAIL("Not yet developed - espconnConnectTestIndex out of range");
        return ESPCONN_ARG;
    }

    return 0;
}
signed char espconn_send(struct espconn *espconn, unsigned char *psent, unsigned short length)
{
    INFO("This mock is called when the TCP session sends data using ESPCONN library");

    espconnSendCalled = true;

    if (espconnSendTestIndex == 0)
    {
        INFO("successful send");
        ip_addr_t ipAddressPassed;
        IP4_ADDR(&ipAddressPassed, espconn->proto.tcp->remote_ip[0],
                 espconn->proto.tcp->remote_ip[1],
                 espconn->proto.tcp->remote_ip[2],
                 espconn->proto.tcp->remote_ip[3]);
        ip_addr_t ipAddressTest;
        IP4_ADDR(&ipAddressTest, IP_1, IP_2, IP_3, IP_4 + ipOffset);
        REQUIRE_EQ(ipAddressPassed.addr, ipAddressTest.addr);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        REQUIRE_EQ(strcmp((const char *)psent, "This is a test message"), 0);
        REQUIRE_EQ(length, strlen("This is a test message"));
        memcpy(&testEspconn, espconn, sizeof(testEspconn)); // save the espconn;
        return 0;
    }
    else if (espconnSendTestIndex == 1)
    {
        FAIL("return a generic arg issue");
        return ESPCONN_ARG;
    }
    else if (espconnSendTestIndex == 2)
    {
        FAIL("return an out-of-memory issue");
        return ESPCONN_MEM;
    }
    else if (espconnSendTestIndex == 3)
    {
        FAIL("return a buffer is full issue");
        return ESPCONN_MAXNUM;
    }
    else
    {
        FAIL("Not yet developed - espconnSendTestIndex out of range");
        return ESPCONN_ARG;
    }

    return 0;
}

signed char espconn_disconnect(struct espconn *espconn)
{
    INFO("This mock is called when the TCP Server disconnects a session using ESPCONN library");

    espconnDisconnectCalled = true;

    if (espconnDisconnectTestIndex == 0)
    {
        INFO("successful disconnection");
        ip_addr_t ipAddressPassed;
        IP4_ADDR(&ipAddressPassed, espconn->proto.tcp->remote_ip[0],
                 espconn->proto.tcp->remote_ip[1],
                 espconn->proto.tcp->remote_ip[2],
                 espconn->proto.tcp->remote_ip[3]);
        ip_addr_t ipAddressTest;
        IP4_ADDR(&ipAddressTest, IP_1, IP_2, IP_3, IP_4);
        REQUIRE_EQ(ipAddressPassed.addr, ipAddressTest.addr);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        return 0;
    }
    else if (espconnDisconnectTestIndex == 1)
    {
        FAIL("return a generic arg issue to espconn disconnect");
        ip_addr_t ipAddressPassed;
        IP4_ADDR(&ipAddressPassed, espconn->proto.tcp->remote_ip[0],
                 espconn->proto.tcp->remote_ip[1],
                 espconn->proto.tcp->remote_ip[2],
                 espconn->proto.tcp->remote_ip[3]);
        ip_addr_t ipAddressTest;
        IP4_ADDR(&ipAddressTest, IP_1, IP_2, IP_3, IP_4);
        REQUIRE_EQ(ipAddressPassed.addr, ipAddressTest.addr);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        return ESPCONN_ARG;
    }
    else
    {
        FAIL("Not yet developed - espconnDisconnectTestIndex out of range");
        return ESPCONN_ARG;
    }

    return 0;
}

signed char espconn_abort(struct espconn *espconn)
{
    INFO("This mock is called to trigger a session abort");

    espconnAbortTestCalled = true;

    if (espconnAbortTestIndex == 0)
    {
        INFO("successful abortion");
        ip_addr_t ipAddressPassed;
        IP4_ADDR(&ipAddressPassed, espconn->proto.tcp->remote_ip[0],
                 espconn->proto.tcp->remote_ip[1],
                 espconn->proto.tcp->remote_ip[2],
                 espconn->proto.tcp->remote_ip[3]);
        ip_addr_t ipAddressTest;
        IP4_ADDR(&ipAddressTest, IP_1, IP_2, IP_3, IP_4);
        REQUIRE_EQ(ipAddressPassed.addr, ipAddressTest.addr);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        return 0;
    }
    else if (espconnAbortTestIndex == 1)
    {
        FAIL("return a generic arg issue to espconn abort");
        ip_addr_t ipAddressPassed;
        IP4_ADDR(&ipAddressPassed, espconn->proto.tcp->remote_ip[0],
                 espconn->proto.tcp->remote_ip[1],
                 espconn->proto.tcp->remote_ip[2],
                 espconn->proto.tcp->remote_ip[3]);
        ip_addr_t ipAddressTest;
        IP4_ADDR(&ipAddressTest, IP_1, IP_2, IP_3, IP_4);
        REQUIRE_EQ(ipAddressPassed.addr, ipAddressTest.addr);
        REQUIRE_EQ(espconn->proto.tcp->remote_port, TEST_PORT_1);
        return ESPCONN_ARG;
    }
    else if (espconnAbortTestIndex == 99)
    {
        FAIL("Abort called unexpectedly");
        return ESPCONN_ARG;
    }
    else
    {
        FAIL("Not yet developed - espconnAbortTestIndex out of range");
        return ESPCONN_ARG;
    }

    return 0;
}
