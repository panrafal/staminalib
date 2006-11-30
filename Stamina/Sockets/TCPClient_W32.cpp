#include "stdafx.h"
#include "TCPClient_W32.h"

namespace Stamina {

  TCPClient_W32::TCPClient_W32(SOCKET socket, sockaddr_in service): WinSocket(1, 1) {
    if (!socket && socket == INVALID_SOCKET) {
      throw InvalidSocketException();
    }
    _threads = new ThreadRunnerStore();

    _socket = socket;
    // _host ??
    //_port = service;
    _state = stDisconnected;

    // set socket to nonblocking mode
    int iMode = 1;
    ioctlsocket(_socket, FIONBIO, (u_long*)&iMode);

    // create loop thread
    _threads->run(boost::bind(&TCPClient_W32::loop, this), "TCPClient_W32::loop");  
  }

  bool TCPClient_W32::connect(const StringRef &host, Port port) {
    LockerCS locker(_critical);

    if (_state != stDisconnected)
      return false;

    _state = stConnecting;
    _host = host;
    _port = port;
    _threads->run(boost::bind(&TCPClient_W32::connecting, this), "Socket::connecting");
    return true;
  }

  unsigned int TCPClient_W32::connecting() {
    hostent *hp;
    unsigned long addr;
    struct sockaddr_in server;
    unsigned long ul = 1;

    _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    _state = stConnecting;

    if (_socket == INVALID_SOCKET) {
      evtOnError(SocketError(SocketError::etWSAError, WSAGetLastError()));
      _state = stDisconnected;
      ExitThread(-1);
    }

    /*
    if (inet_addr(_host.c_str()) == INADDR_NONE) {
      hp = gethostbyname(_host.c_str());
    } else {
      addr = inet_addr(_host.c_str());
      hp = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);
    }
    */
    hp = gethostbyname(_host.c_str());

    if (hp == NULL) {
      closesocket(_socket);
      evtOnError(SocketError(SocketError::etWSAError, WSAGetLastError()));
      _state = stDisconnected;
      ExitThread(-1);
    } else {
      // server.sin_addr.s_addr =*((unsigned long*)hp->h_addr);
      server.sin_addr.s_addr =((struct in_addr *)(hp->h_addr))->s_addr;
      server.sin_family = AF_INET;
      server.sin_port = htons( _port );

      // set to send keep-alives.
      long timeout = getTimeOut();
      setsockopt(_socket, SOL_SOCKET, SO_KEEPALIVE,(char*)&timeout, sizeof(timeout));
      // set receives time-out in milliseconds
      setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO,(char*)&timeout, sizeof(timeout));
      // set send time-out in milliseconds
      setsockopt(_socket, SOL_SOCKET, SO_SNDTIMEO,(char*)&timeout, sizeof(timeout));

      _threads->run(boost::bind(&TCPClient_W32::loop, this), "TCPClient_W32::loop");

      if (::connect(_socket, (const sockaddr*)&server, sizeof(sockaddr_in)) == SOCKET_ERROR &&
        WSAGetLastError() != WSAEWOULDBLOCK) {
        closesocket(_socket);
        evtOnError(SocketError(SocketError::etWSAError, WSAGetLastError()));
        this->_state = stDisconnected;
        ExitThread(-1);
      }
      // est socket to nonblocking mode
      int iMode = 1;
      ioctlsocket(_socket, FIONBIO, (u_long*)&iMode);
    }
    ExitThread(NO_ERROR);
  }

  unsigned int TCPClient_W32::loop() {
    fd_set writefd;
    fd_set readfd;
    fd_set exceptfd;
    timeval time;
    
    time.tv_sec = getTimeOut() / 1000 ;//100;
    time.tv_usec = getTimeOut() % 1000; 

    while (_state != stDisconnected && _state != stDisconnecting) {

      if (_state == stConnecting) {
        FD_ZERO(&writefd); FD_SET(_socket, &writefd);
        FD_ZERO(&exceptfd); FD_SET(_socket, &exceptfd);
        
        // determine socket status
        int ret = select(_socket, NULL, &writefd, &exceptfd, &time);

        // if error appears, fire signal and terminate thread
        if (ret < 1) {
          evtOnError(SocketError(SocketError::etWSAError, WSAGetLastError()));
          _state = stDisconnected;
          return -1;
        } else if (ret > 0) {
          // yeah!! we are connected;)
          if (FD_ISSET(_socket, &writefd)) {
            _state = stConnected;
            evtOnConnected();
          }
          // connection attempt faild so fire proper signal and terminate thread
          if (FD_ISSET(_socket, &exceptfd)) {
            evtOnClose();
            _state = stDisconnected;
            return -1;
          }
        }
      } else if (_state == stConnected) {
        FD_ZERO(&readfd); FD_SET(_socket, &readfd);

        // determine socket status
        int ret = select(_socket, &readfd, NULL, NULL, &time);

        // if error appears, fire signal and terminate thread
        if (ret == SOCKET_ERROR) {
          evtOnError(SocketError(SocketError::etWSAError, WSAGetLastError()));
          _state = stDisconnected;
          return -1;
        } else if (ret > 0) {
          // some data arrive
          if (FD_ISSET(_socket, &readfd)) {
            char b[8];
            int recv_ret = recv(_socket, (char*)b, 8, MSG_PEEK);
            if (recv_ret > 0) {
              evtOnReceived();
            } else if (recv_ret == 0) {
              _state = stDisconnected;
              closesocket(_socket);
              _socket = NULL;
              evtOnClose();
            } else {
              evtOnError(SocketError(SocketError::etWSAError, WSAGetLastError()));
              _state = stDisconnected;
              return -1;
            }
          }
        }
      }
      Sleep(5);
    }
    ExitThread(NO_ERROR);
  }

  void TCPClient_W32::close() {
    LockerCS locker(_critical);

    if (_state == stDisconnected)
      return;

    if (_socket && _socket != INVALID_SOCKET)
    {
      LINGER lin = {0};
      lin.l_linger = 0;
      lin.l_onoff = 1;
      setsockopt(_socket, SOL_SOCKET, SO_LINGER, (const char*)&lin, sizeof(lin));
      _state = stDisconnected;
      closesocket(_socket);
      _socket = NULL;
    }
  }

  int TCPClient_W32::write(const char *data, Size size) {
    LockerCS locker(_critical);
    if (size > 0) {
      unsigned int bytes = 0;
      unsigned int sent = 0;

      while (sent < size) {
        sent = ::send(_socket, data + bytes, size - bytes, 0);
        if (sent == WSAEWOULDBLOCK)
          Sleep(100);
        else if (sent < 0)
          throw WSASocketException(WSAGetLastError());
        bytes += sent;
      }
    }
    return 0;
  }

  int TCPClient_W32::read(char *data, Size size) {
    if (size < 1) {
      return 0;
    }
    
    int ret = recv(_socket, data, size, NULL);
    if (ret < 0)
      throw WSASocketException(WSAGetLastError());
    return ret;
  }
}