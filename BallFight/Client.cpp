#include "Client.h"
#include "Networking.h"

#include "Windowsx.h"
#include "SceneManager.h"

Client g_client;

Client::Client()
{
    m_commandHandlers[ASSIGN_ID] = new AssignIdHandler();
    m_commandHandlers[SHOOT_BALL] = new ShootBallHandler();
    m_commandHandlers[NEW_PLAYER] = new NewPlayerHandler();
    m_commandHandlers[MOVE_TO] = new MoveToHandler();
    m_commandHandlers[GAME_OVER] = new GameOverHandler();

    m_commandHandlers[NEW_MONSTER] = new NewMonsterHandler();
    m_commandHandlers[MOVE_MONSTER_TO] = new MoveMonsterToHandler();
    m_commandHandlers[KILL_MONSTER] = new KillMonsterHandler();
}

Client::~Client()
{}

Client& Client::Get()
{
    return g_client;
}

void Client::Update(int ms)
{
    ProcessMessage();
    UpdateWorld(ms);
}

void Client::UpdateWorld(int ms)
{
    m_world.Update(ms);
}

ClientWorld& Client::GetWorld()
{
    return m_world;
}


void Client::ProcessMessage()
{
    Message* received = Networking::Get().GetClientFromComms();
    if (received == nullptr) return;

    if (received->type != Message::SERVER) {
        // TODO: log
        OutputDebugString(L"Unexpected message type!\n");
        return;
    }

    ServerMessage* serverReceived = static_cast<ServerMessage*>(received);
    m_commandHandlers[serverReceived->rpcMessage->command]->Handle(serverReceived, &m_world);
    
    delete serverReceived->rpcMessage;
    delete serverReceived;
}

void Client::OnLButtonDown(LPARAM lParam)
{ 
    if (!m_world.IsLocalPlayerAlive()) return;

    RpcMessage* msg = new MoveRequest(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)));
    ClientMessage* clientMsg = new ClientMessage(m_world.GetId(), msg);
    Networking::Get().PostToComms(clientMsg);
}

void Client::OnRButtonDown(LPARAM lParam)
{
    if (!m_world.IsLocalPlayerAlive()) return; 

    RpcMessage* msg = new ShootRequest(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)));
    ClientMessage* clientMsg = new ClientMessage(m_world.GetId(), msg);
    Networking::Get().PostToComms(clientMsg);
}

