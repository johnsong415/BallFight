#include "Networking.h"


Networking g_networking;


Networking::Networking()
{
    m_timeToWait.tv_sec = TIMEOUT_SECS;
    m_timeToWait.tv_usec = TIMEOUT_USECS;

    FD_ZERO(&m_clientSocketFdSet);
    FD_ZERO(&m_listeningSocketSet);
}

Networking::~Networking()
{}


Networking& Networking::Get()
{
    return g_networking;
}

void Networking::Initialize(const std::string& ipAddress)
{
    g_networking.m_ipAddress = ipAddress;
    m_pThread = new std::thread(
        []() { Networking::Get().ThreadMain(); });
}

void Networking::StartServer()
{
    // start server here
    m_serverStarted = true;
    m_listeningSocket = new Socket(INVALID_SOCKET);
    StartUpServerSocket(*m_listeningSocket);

}

void Networking::StartClient()
{
    m_clientStarted = true;
    m_clientSocket = new Socket(INVALID_SOCKET);
    StartUpClientSocket(*m_clientSocket);
}

void Networking::StopServer()
{
    // TODO: implement server socket close and accepted sockets close
}

void Networking::StopClient()
{
    // TODO: implement client socket close
}


void Networking::ProcessCommands()
{
    // pop only one, change for all,
    Message* cmd = m_cmdMainToComm.Pop();
    if (cmd != nullptr) {
        InternalMessage* poppedMsg = static_cast<InternalMessage*>(cmd);
        InternalMessage* interCmd = new InternalMessage;
        interCmd->callback = poppedMsg->callback;
        
        // TODO: unordered map
        switch (poppedMsg->command) {
        case START_SERVER:
            StartServer();
            interCmd->command = START_SERVER_ACK;
            break;

        case START_CLIENT:
            StartClient();
            interCmd->command = START_CLIENT_ACK;
            break;

        case STOP_SERVER:
            StopServer();
            break;
        case STOP_CLIENT:
            StopClient();
            break;
        }

        m_cmdCommToMain.Enqueue(interCmd);

        delete poppedMsg;
    }
}

void Networking::ProcessClient()
// client socket (as client)
{
    if (!m_clientStarted) return;

    // dequeue from client send q => send
    RpcMessage* toSend = static_cast<RpcMessage*>(m_clientMainToComm.Pop());
    if (toSend != nullptr) SendMsg(m_clientSocket, toSend);
    

    // recv => enqueue to client recv q
    fd_set clientSet;
    FD_ZERO(&clientSet);
    FD_SET(static_cast<SOCKET>(*m_clientSocket), &clientSet);

    int ready = select(0, &clientSet, nullptr, nullptr, &m_timeToWait);
    if (ready == 1) {
        RpcMessage* toRecv = nullptr;
        RecvMessage(m_clientSocket, &toRecv);
        ServerMessage* serverMsg = new ServerMessage(toRecv);

        m_clientCommToMain.Enqueue(serverMsg);
    }

    
}

void Networking::ProcessServer()
// listen socket & accepted sockets
{
    if (!m_serverStarted) return;

    // dequeue from server send q => send
    // to which client?
    // send here
    RpcMessage* toSend = static_cast<RpcMessage*>(m_serverMainToComm.Pop());
    if (toSend != nullptr) {
        for (auto&& [_, socket] : m_sockets) {
            // TODO: figure out => structure binding
            SendMsg(socket, toSend);
        }
        delete toSend;
    }

    fd_set listeningSet;
    FD_ZERO(&listeningSet);
    FD_SET(static_cast<SOCKET>(*m_listeningSocket), &listeningSet);

    int ready = select(0, &listeningSet, nullptr, nullptr, &m_timeToWait);
    // TODO: error handling
    if (ready == 1) {
        Socket* accepted = new Socket(INVALID_SOCKET);
        Accept(*m_listeningSocket, *accepted);
    }

    fd_set recvSet;
    FD_ZERO(&recvSet);
    for (auto kvpair : m_playerIds) {
        FD_SET(kvpair.first, &recvSet);
    }
    
    ready = select(0, &recvSet, nullptr, nullptr, &m_timeToWait);
    // TODO: error handling
    for (auto kvpair : m_playerIds) {
        if (FD_ISSET(kvpair.first, &recvSet)) {
            RpcMessage* toRecv = nullptr;
            RecvMessage(m_sockets.at(kvpair.second), &toRecv);

            ClientMessage* clientMsg = new ClientMessage(kvpair.second, toRecv);
            m_serverCommToMain.Enqueue(clientMsg);
        }
    }

    // recv: accept?
    // recv: recv?
}


void Networking::ThreadMain()
{
    while (!m_shutdown) {

        // TODO: when SHUTDOWN command wwas received, break

        ProcessCommands(); // dequeue => internal commands

        ProcessServer(); // listen socket & accepted sockets

        ProcessClient(); // client socket (as client)
        
    }

}

bool Networking::SendMsg(Socket* socket, RpcMessage* toSend)
{
    PacketBuffer messageBuffer;

    int size = 0;

    toSend->Serialize(messageBuffer, size);

    int sizeSent = socket->Send(messageBuffer.GetBuffer(), sizeof(int) + size);
    int totalSizeSent = sizeSent;
    while (totalSizeSent < size) {
        if (sizeSent == SOCKET_ERROR) {
            int error = WSAGetLastError();
            MessageBox(NULL, L"Failed to send", L"Oops!", MB_OK);
            return false;
        }
        sizeSent = socket->Send(&messageBuffer.GetBuffer()[totalSizeSent], size - totalSizeSent);
        totalSizeSent += sizeSent;
    }

    return true;
}

bool Networking::RecvMessage(Socket* socket, RpcMessage** toReceive)
{
    int size = 0;
    socket->Recv(reinterpret_cast<char*>(&size), sizeof(int));
    // TODO: in case this hasn't been received fully, do...

    PacketBuffer messageBuffer;
    int received = socket->Recv(messageBuffer.GetBuffer(), size);
    int totalReceived = received;
    while (totalReceived < size) {
        if (received == SOCKET_ERROR) {
            int error = WSAGetLastError();
            MessageBox(NULL, L"Failed to send", L"Oops!", MB_OK);
            return false;
        }
        size = socket->Recv(&messageBuffer.GetBuffer()[totalReceived], size - totalReceived);
        totalReceived += size;
    }

    RpcCommand msgCommand;
    
    messageBuffer.Read(msgCommand);

    // TODO: unordered map for t
    if (msgCommand == SHOOT_REQUEST) {
        *toReceive = new ShootRequest();
    }
    else if (msgCommand == SHOOT_BALL) {
        *toReceive = new ShootBall();
    }
    else if (msgCommand == MOVE_REQUEST) {
        *toReceive = new MoveRequest();
    }
    else if (msgCommand == MOVE_TO) {
        *toReceive = new MoveTo();
    }
    else if (msgCommand == NEW_PLAYER) {
        *toReceive = new NewPlayer();
    }
    else if (msgCommand == ASSIGN_ID) {
        *toReceive = new AssignID(0);
    }
    else if (msgCommand == GAME_OVER) {
        *toReceive = new GameOver();
    }
    else if (msgCommand == NEW_MONSTER) {
        *toReceive = new NewMonster();
    }
    else if (msgCommand == MOVE_MONSTER_TO) {
        *toReceive = new MoveMonsterTo();
    }
    else if (msgCommand == KILL_MONSTER) {
        *toReceive = new KillMonster();
    }

    (*toReceive)->Parse(messageBuffer);


    return true;
}

bool Networking::StartUpServerSocket(Socket& serverSocket)
{
    serverSocket.Bind("0.0.0.0", LISTENING_PORT);
    serverSocket.Listen();

    return true;
}

bool Networking::StartUpClientSocket(Socket& clientSocket)
{
    clientSocket.Connect(m_ipAddress.c_str(), LISTENING_PORT);
    int error = WSAGetLastError();

    return true;
}

bool Networking::Accept(Socket& serverSocket, Socket& acceptedSocket)
{
    Socket* accepted = new Socket(INVALID_SOCKET);
    m_listeningSocket->Accept(*accepted);
    
    
    m_sockets[m_nextClientId] = accepted;
    m_playerIds[(SOCKET) *accepted] = m_nextClientId;

    InternalMessage* acceptedMsg = new InternalMessage;
    acceptedMsg->command = CLIENT_ACCEPTED;
    acceptedMsg->callback = nullptr;
    acceptedMsg->flag = m_nextClientId;
    ++m_nextClientId;

    m_cmdCommToMain.Enqueue(acceptedMsg);

    RpcMessage* assignId = new AssignID(acceptedMsg->flag);
    SendMsg(accepted, assignId);

    return true;
}

void Networking::PostToComms(Message* msg)
{
    if (msg->type == Message::INTERNAL) {
        m_cmdMainToComm.Enqueue(msg);
    }
    else if (msg->type == Message::SERVER) {
        ServerMessage* serverMsg = static_cast<ServerMessage*>(msg);
        RpcMessage* actualMessage = serverMsg->rpcMessage;
        m_serverMainToComm.Enqueue(actualMessage);
        delete serverMsg;
    }
    else if (msg->type == Message::CLIENT) {
        ClientMessage* clientMsg = static_cast<ClientMessage*>(msg);
        RpcMessage* actualMessage = clientMsg->message;
        m_clientMainToComm.Enqueue(actualMessage);
        delete clientMsg;
    }
}

Message* Networking::GetCmdFromComms()
{
    return m_cmdCommToMain.Pop();
}

Message* Networking::GetClientFromComms()
{
    return m_clientCommToMain.Pop();
}

Message* Networking::GetServerFromComms()
{
    return m_serverCommToMain.Pop();
}

