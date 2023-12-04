#define DOCTEST_CONFIG_IMPLEMENT // REQUIRED: Enable custom main()
#define DOCTEST_THREAD_LOCAL

#include <functional>

#include <doctest.h>

#include "scenario_01_instantiation.h"
#include "scenario_02_start_tcp_server.h"
#include "scenario_03_start_tcp_client.h"
#include "scenario_04_session_established.h"
#include "scenario_05_send_message.h"
#include "scenario_06_rcv_message.h"
#include "scenario_07_disconnect.h"

int main(int argc, char **argv)
{
    doctest::Context context;

    // BEGIN:: PLATFORMIO REQUIRED OPTIONS
    context.setOption("success", true);     // Report successful tests
    context.setOption("no-exitcode", true); // Do not return non-zero code on failed test case
    // END:: PLATFORMIO REQUIRED OPTIONS

    // YOUR CUSTOM DOCTEST OPTIONS
    context.applyCommandLine(argc, argv);
    return context.run();
}
