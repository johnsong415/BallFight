#include "Socket.h"

#include "framework.h"

#pragma comment(lib, "ws2_32.lib")


Socket::Socket(SOCKET passedSocket)
{
    m_timeout.tv_sec = TIMEOUT_SECS;
    m_timeout.tv_usec = TIMEOUT_USECS;

    if (passedSocket == INVALID_SOCKET) {
        m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    }
    else {
        m_socket = passedSocket;
    }

    if (m_socket == INVALID_SOCKET) MessageBox(NULL, L"Invalid socket", L"Oops!", MB_OK);
}

Socket::~Socket()
{
    Close();
}

void Socket::SetSocketHandle(SOCKET newSocket)
{
    m_socket = newSocket;
}

bool Socket::Accept(Socket& newSocket)
{
    // or receive socket ref from accept
    // return true false based on success
    SOCKET accepted = accept(m_socket, nullptr, nullptr);
    if (accepted == INVALID_SOCKET) return false;
    else {
        newSocket.SetSocketHandle(accepted);
        return true;
    }
}

int Socket::Bind(const std::string& address, int port)
{
    sockaddr_in server;
    // have socket have set ip address and port, use those values for binding, or get the values from bind
    server.sin_addr.s_addr = inet_addr(address.c_str());
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    return bind(m_socket, (SOCKADDR*)&server, sizeof(server));
}

void Socket::Close()
{
    closesocket(m_socket);
}

int Socket::Connect(const char* address, int port)
{
    sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(address);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    return connect(m_socket, (SOCKADDR*) &server, sizeof(server));
}

int Socket::Send(const char* buffer, int size)
{
    return send(m_socket, buffer, size, 0);
}

int Socket::Recv(char* buffer, int size)
{
    return recv(m_socket, buffer, size, 0);
}

int Socket::Listen()
{
    // TODO: magic number! 10/20 default
    return listen(m_socket, 10);
}
// add timeval to class
int Socket::Select(fd_set* readFDS, fd_set* writeFDS)
{
    return select(0, readFDS, writeFDS, nullptr, &m_timeout);
}


class __SocketInitializer__
{
public:
    __SocketInitializer__() {
        WSADATA wsaData;
        auto result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0) MessageBox(NULL, L"Failed to start", L"Oops!", MB_OK);
    }
    ~__SocketInitializer__() {
        WSACleanup();
    }
};

static __SocketInitializer__ __SocketInitializerInstance__;
