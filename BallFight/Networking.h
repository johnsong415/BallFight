#pragma once

#include "CommonTypes.h"

#include "Socket.h"

#include "CommunicationQueue.h"

#include "Messages.h"

#include <unordered_map>


// TODO: Get from command line
#define IP_ADDRESS          "127.0.0.1"
#define LISTENING_PORT      1945


class Networking
{
public:
    Networking();
    ~Networking();

public:
    static Networking& Get();

public:
    void Initialize();
    void Shutdown(); // make m_shutdown false here

    void StartServer();
    void StartClient();
    
    void StopServer();
    void StopClient();

public:
    // Process on networking side
    void ProcessServer();
    void ProcessClient();
    void ProcessCommands();

    //ClientMessage* clientMessage;

public:
    void ThreadMain();

public:
    bool SendMsg(Socket* socket, RpcMessage* toSend);
    bool RecvMessage(Socket* socket, RpcMessage** toReceive);

    bool StartUpClientSocket(Socket& clientSocket);
    bool StartUpServerSocket(Socket& severSocket);

    bool Accept(Socket& serverSocket, Socket& acceptedSocket);

public:
    // Main to Comm
    void PostToComms(Message* msg);
    // Comm to Main
    Message* GetCmdFromComms();
    Message* GetClientFromComms();
    Message* GetServerFromComms();


private:
    // Socket/ID maps, for server use only
    std::unordered_map<ObjectId, Socket*> m_sockets; // keyed by player id
    std::unordered_map<SOCKET, ObjectId> m_playerIds; // keyed by socket handles
    // Comm queues
    CommunicationQueue m_cmdMainToComm;
    CommunicationQueue m_clientMainToComm;
    CommunicationQueue m_serverMainToComm;

    CommunicationQueue m_cmdCommToMain;
    CommunicationQueue m_clientCommToMain;
    CommunicationQueue m_serverCommToMain;
    // Client/Server flags
    bool m_serverStarted = false;
    bool m_clientStarted = false;

    // Server
    Socket* m_listeningSocket = nullptr;
    
    // Client
    Socket* m_clientSocket = nullptr;

    char m_buffer[256];
    
    int m_nextPlayerId = 0;

    fd_set m_listeningSocketSet;
    fd_set m_clientSocketFdSet;

    

    timeval m_timeToWait;

    
    std::thread* m_pThread;
    
    std::atomic_bool m_shutdown = false;


    int m_nextClientId = 0;
};

