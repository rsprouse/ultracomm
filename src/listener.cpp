#include "StdAfx.h"

#define DEFAULT_BUFLEN 512

// See https://msdn.microsoft.com/en-us/library/windows/desktop/bb530742(v=vs.85).aspx
Listener::Listener(const int port)
    : port(port)
{
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    char *hostname = "localhost";

    ZeroMemory(&hints, sizeof (hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the local address and port to be used by the server.
    iResult = getaddrinfo(hostname, port, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        throw SocketAddrinfoError();
    }

    // Create a SOCKET for the server to listen for client connections.
    listen_socket = INVALID_SOCKET;
    listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listen_socket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        throw SocketInvalidError();
    }

    // Set up the TCP listening socket.
    iResult = bind( listen_socket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(listen_socket);
        WSACleanup();
        throw SocketBindError();
    }

    freeaddrinfo(result);
}

/*
    Listen for and accept a client connection.
*/
void Listener::listen()
{
    // Allow connection from a single outside process only.
    const int max_connections = 1;
    if ( listen( listen_socket, max_connections ) == SOCKET_ERROR ) {
        printf( "Listen failed with error: %ld\n", WSAGetLastError() );
        closesocket(listen_socket);
        WSACleanup();
        throw SocketListenError();
    }

    // Listen for and accept a single connection.
    client_socket = INVALID_SOCKET;
    client_socket = accept(listen_socket, NULL, NULL);
    if (client_socket == INVALID_SOCKET) {
        printf("accept failed: %d\n", WSAGetLastError());
        closesocket(listen_socket);
        WSACleanup();
        throw SocketAcceptError();
    }
}

/*
    Block until client gracefully closes the connection.
*/
void Listener::block()
{
    
    char recvbuf[DEFAULT_BUFLEN];
    int iResult, iSendResult;
    int recvbuflen = DEFAULT_BUFLEN;
    
    // Receive until the peer shuts down the connection
    do {
    
        iResult = recv(client_socket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            // We would do something here if we were interested in the content of
            // received messages.

/*
            printf("Bytes received: %d\n", iResult);
    
            // Echo the buffer back to the sender
            iSendResult = send(client_socket, recvbuf, iResult, 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed: %d\n", WSAGetLastError());
                closesocket(client_socket);
                WSACleanup();
                throw SocketSendError();
            }
            printf("Bytes sent: %d\n", iSendResult);
*/
        } else if (iResult == 0)
            printf("Connection closing...\n");
        else {
            printf("recv failed: %d\n", WSAGetLastError());
            closesocket(client_socket);
            WSACleanup();
            throw SocketReceiveError();
        }
    
    } while (iResult > 0);
    
}

/*
    Disconnect and shut down the server.
*/
void Listener::shutdown()
{
    // Shut down the send half of the connection since no more data will be sent.
    iResult = shutdown(client_socket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(client_socket);
        WSACleanup();
        throw SocketShutdownError();
    }

    // Clean up.
    closesocket(client_socket);
    closesocket(listen_socket);
    WSACleanup();
}
