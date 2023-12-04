# TCP Server and Session Implementation

## Table of Contents

- [TCP Server](#tcp-server)
  - [Overview](#overview)
  - [Usage](#usage)
  - [License](#license)

- [TCP Session](#tcp-session)
  - [Overview](#overview-1)
  - [Usage](#usage-1)
  - [Callbacks](#callbacks)
  - [Static Utility Methods](#static-utility-methods)
  - [License](#license-1)

---

## TCP Server

### Overview

The `TcpServer` class abstracts the concept of a TCP server from the underlying firmware and hardware. This server operates on the transport layer of the Internet Protocol (IP) suite. It can either run in _Server Mode_, listening for incoming TCP connections from multiple clients and responding to their requests, or _CLient Mode_, in which case it sets up a connection to a remote server. The implementation is designed for ESP8266 (ESP01) devices and uses the ESPCONN library.

### Usage

To use the `TcpServer`, follow these steps:

1. Include the necessary headers:

    ```cpp
    #include "tcp_server.h"
    ```

2. Create an instance of the server:

    ```cpp
    TcpServer &server = TcpServer::getInstance();
    ```

3. Start the server as a passive listener for incomming connections (server mode). 
   _port_: the incoming port on which the server listens
   _callback_: the method that is called when an incoming session is established
   _object_: the object that owns the instatiation of this server

    ```cpp
    server.startTcpServer(port, callback, object);
    ```

    OR Start the server as an active outgoing connection (client mode).
    _ipaddress_: the IP address of hte remote server
    _port_ : the port of the remote server
    _callback_: the method that is called when the connection is established
    _object_: the object that owns the instatiation of this server

    ```cpp
    bool startTcpClient(ipAddress, port, callback, object);
    ```

4. In both cases a shared pointer to a TCP Session is returned in the callback. The session
   requires the listener to register a callback owner and callbacks to handle disonnect, 
   an incoming messag and confirmation that a message has been sent.  

### License

This implementation is provided under the MIT License. See the [LICENSE](LICENSE) section for details.

---

## TCP Session

### Overview

The `TcpSession` class represents a TCP session, covering the entire lifespan of a TCP connection, including setup, data exchange, and teardown phases. It provides an abstraction over the ESP8266 firmware for managing TCP sessions. A session is only cfreated by
TcpServer and returned in the connected callback. 

### Usage

To use the `TcpSession`, follow these steps:

1. Include the necessary headers:

    ```cpp
    #include "tcp_session.h"
    ```

2. When informed by the TcpServer, then register callbacks for session events:

    ```cpp
    session.registerSessionDisconnectCb(disconnectCallback);
    session.registerIncomingMessageCb(incomingMessageCallback);
    session.registerMessageSentCb(messageSentCallback);
    ```

3. Handle session events through registered callbacks.

### Callbacks

- `disconnectCallback`: Invoked when the session is disconnected.
- `incomingMessageCallback`: Invoked when an incoming message is received.
- `messageSentCallback`: Invoked when a message is successfully sent.

### Static Utility Methods

The `TcpSession` class provides static utility methods for IP address conversion and unique identifier creation.

- `convertIpAddress`: Converts an array of unsigned characters to an IP address.
- `createUniqueIdentifier`: Creates a unique identifier for a session based on IP address and port.

### License

This implementation is provided under the MIT License. See the [LICENSE](LICENSE) file for details.

---

