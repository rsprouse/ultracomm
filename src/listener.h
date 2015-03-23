#pragma once

/*
    The purpose of the Listener class is to open a socket and accept a
    connection from another local process. Use block() in the current
    process to block execution until the outside process drops the
    connection.
*/

class Listener
{
    const char *port;           // socket port
    SOCKET listen_socket;     // socket listener
    SOCKET client_socket;     // client socket

public:

    struct SocketAddrinfoError : public std::exception
    {
      const char * what () const throw ()
        {
          return "Could not resolve local address and port.";
        }
    };
  
    struct SocketInvalidError : public std::exception
    {
      const char * what () const throw ()
        {
          return "Invalid socket.";
        }
    };
  
    struct SocketBindError : public std::exception
    {
      const char * what () const throw ()
        {
          return "Could not bind to socket.";
        }
    };
  
    struct SocketListenError : public std::exception
    {
      const char * what () const throw ()
        {
          return "Could not listen on socket.";
        }
    };
  
    struct SocketAcceptError : public std::exception
    {
      const char * what () const throw ()
        {
          return "Could not accept connection on socket.";
        }
    };
  
    struct SocketBlockError : public std::exception
    {
      const char * what () const throw ()
        {
          return "Socket error while blocking.";
        }
    };
  
    struct SocketSendError : public std::exception
    {
      const char * what () const throw ()
        {
          return "Socket error while sending data.";
        }
    };
  
    struct SocketReceiveError : public std::exception
    {
      const char * what () const throw ()
        {
          return "Socket error while receiving data.";
        }
    };
  
    struct SocketShutdownError : public std::exception
    {
      const char * what () const throw ()
        {
          return "Socket error while shutting down.";
        }
    };
  
    Listener(const char *port);
    void do_listen();
    void do_block();
    void do_shutdown();
};
