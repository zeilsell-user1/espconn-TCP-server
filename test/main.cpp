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

// Test cases implemented in this collection:
//
// Adding and Removing Sessions:
// 2: Add a session and ensure it's present in the server's session list.
// 3: Remove a session and verify that it's no longer in the list.
// 4: Attempt to remove a non-existent session and handle the case appropriately.
//
// Session Callbacks:
// 5: Register various callback functions for a session and ensure they are properly invoked.
// 6: Test the session disconnect callback when a session is disconnected.
//
// Handling Connections:
// 7: Test the server's ability to accept incoming connections.
// 8:Verify that the server handles client disconnections appropriately.
//
// Session Lookup:
// 9: Retrieve a session using its ID and ensure it's the correct session.
// 10: Attempt to retrieve a non-existent session and handle the case appropriately.
//
// Multiple Sessions:
// 11: Add multiple sessions to the server and ensure they are all tracked correctly.
// 12: Test the server's behavior with a high number of concurrent sessions.
//
// Error Handling:
// 13: Test the server's behavior when encountering errors during operations.
// 14: Check how the server handles unexpected events, such as receiving data without a corresponding session.
//
// Client Connection:
// 15: Test the server's ability to initiate connections to other servers.
// 16: Verify that the server can handle disconnections in this scenario.
//
// Callback Ownership:
// 17: Ensure that the callbacks retain the correct ownership information (i.e., the upper layer object).
//
// Session Expiry:
// 18: Test the behavior of sessions with different expiry intervals.
// 19: Check if sessions are correctly expired after reaching their timeout.
//
// IPv6 Support:
// 20: Check it is not supported ATM
//
// Edge Cases:
// 21: Zero Port: Test the server's behavior when starting with port number 0.
// 22: Maximum Port Number: Use the highest possible port number and verify the server's response.
// 23: Invalid IP Address: Attempt to start the server with an invalid IP address and check how it handles the error.
// 24: Null Callbacks: Register null callbacks and ensure the server handles them appropriately.
// 25: Zero Expiry Interval: Set the expiry interval of a session to zero and verify its behavior.
// 26: Negative Expiry Interval: Test how the server handles sessions with negative expiry intervals.
// 27: Overflow Session ID: Test the server's behavior when the session ID reaches its maximum value and overflows.
// 28: Invalid Session Lookup: Attempt to retrieve a session with an invalid (out-of-bounds) session ID.
// 29: Extreme Number of Sessions: Add a very large number of sessions to the server and assess its performance and resource usage.
// 30: Multiple Callback Registrations: Register the same callback multiple times and ensure it doesn't result in issues.
// 31: Unexpected Disconnects: Simulate unexpected disconnects during various stages of a session's lifecycle.
// 32: Unusual Protocol States: Place a session in a protocol state that it shouldn't be in and check how the server reacts.
// 33: Invalid Initialization Order: Test the behavior of the server when methods are called in an unexpected order.
// 34: IPv4/IPv6 Compatibility: If applicable, test the server's behavior when handling both IPv4 and IPv6 connections.
// 35: Resource Exhaustion: Stress test the server by attempting to create sessions beyond the available resources.
// 36: Network Partition: Simulate network partitions and reconnects to evaluate the server's resilience.
// 37: Session Lookup Race Conditions: Introduce race conditions in session lookup and verify the server's thread safety.
// 38: Mixed Session Types: Mix different types of sessions (e.g., server-initiated and client-initiated) and observe the server's behavior.
// 39: Repeated Server Start/Stop: Start and stop the server multiple times in rapid succession and ensure it remains stable.
// 40: Unusual Characters in IP Address: Include non-numeric characters in the IP address and verify how the server handles them.


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
