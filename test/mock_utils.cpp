
#include <stdbool.h>
#include "mock_espconn_functions.h"
#include "mock_espconn_regist_cb.h"
#include "mock_owner_cb_functions.h"
#include "mock_utils.h"

void setupTest()
{
    espconnAcceptTestIndex = 0;
    espconnConnectTestIndex = 0;
    espconnSendTestIndex = 0;
    espconnDisconnectTestIndex = 0;
    espconnAbortTestIndex = 0;
    espconnRegistSentCbTestIndex = 0;
    espconnRegistRecvCbTestIndex = 0;
    espconnRegistReconnectCbTestIndex = 0;
    espconnRegistDisconnectCbTestIndex = 0;
    espconnAbortTestIndex = 99;

    espconnAcceptCalled = false;
    espconnConnectCalled = false;
    espconnSendCalled = false;
    espconnDisconnectCalled = false;
    espconnAbortTestCalled = false;

    espconnRegistConnectCbCalled = false;
    espconnRegistDisconnectCbCalled = false;
    espconnRegistRecvCbCalled = false;
    espconnRegistSentCbCalled = false;
    espconnRegistReconnectCbCalled = false;

    connectedCbCalled = false;
    disconnectedCbCalled = false;
    reconnectCbCalled = false;
    sentCbCalled = false;
    receivedCbCalled = false;

    ipOffset = 0;
}