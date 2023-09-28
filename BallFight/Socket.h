#pragma once

#include <winsock.h>
#include <string>

#include "CommonTypes.h"

#define TIMEOUT_SECS    0
#define TIMEOUT_USECS   10

class Socket
{
public:
    Socket(SOCKET passedSocket);
    ~Socket();

public:
    bool Accept(Socket& newSocket);
    int Bind(const std::string& address, int port);
    void Close();
    int Connect(const char* address, int port);
    int Send(const char* buffer, int size);
    int Recv(char* buffer, int size);
    int Listen();
    int Select(fd_set* readFDS, fd_set* writeFDS);

    operator SOCKET() { return m_socket; }

private:
    void SetSocketHandle(SOCKET newSocket);


private:
    SOCKET m_socket;
    timeval m_timeout;
};

