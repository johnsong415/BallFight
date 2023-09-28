#include "Application.h"

#include "Client.h"
#include "Server.h"
#include "SceneManager.h"
#include "Networking.h"

Application g_application;


Application::Application()
{
}

Application::~Application()
{}

void Application::Initialize(HWND hWnd)
{
    Networking::Get().Initialize();
    Graphics::Initialize(hWnd);

    m_lastTickCount = GetTickCount();;
}

void Application::Update()
{
    // TODO: change QueryPerformanceCounter
    DWORD currentTickCount = GetTickCount();
    if (currentTickCount == m_lastTickCount) return;

    ProcessCommands();
    if (m_server) Server::Get().Update(currentTickCount - m_lastTickCount);
    if (m_client) Client::Get().Update(currentTickCount - m_lastTickCount);
    m_lastTickCount = currentTickCount;
    SceneManager::Get().Update();
}

void Application::ProcessCommands()
{
    Message* popped = Networking::Get().GetCmdFromComms();
    if (popped == nullptr) return;

    InternalMessage* internalMsg = reinterpret_cast<InternalMessage*>(popped);

    switch (internalMsg->command) {
    case CLIENT_ACCEPTED:
        Server::Get().AddPlayer(internalMsg->flag);
        break;

    case START_SERVER_ACK:
        internalMsg->callback(0);
        break;

    case START_CLIENT_ACK:
        internalMsg->callback(0);
        break;
    }

    delete popped;
}

Application& Application::Get()
{
    return g_application;
}

void Application::StartServer()
{
    if (m_started) return;
    m_server = true;
    InternalMessage* startServer = new InternalMessage;
    startServer->command = START_SERVER;
    startServer->callback = OnServerStarted;
    Networking::Get().PostToComms(startServer);

}

void Application::StartClient()
{
    if (m_started) return;
    m_started = true;
    m_client = true;

    SceneManager::Get().SwitchScene(SceneManager::SceneType::INGAME);

    InternalMessage* startClient = new InternalMessage;
    startClient->command = START_CLIENT;
    startClient->callback = OnClientStarted;

    Networking::Get().PostToComms(startClient);
}

void Application::OnServerStarted(ErrorCode errorCodeServer)
{
    // TODO: notify?

    g_application.StartClient();
}

void Application::OnClientStarted(ErrorCode errorCodeClient)
{
    // TODO: notify?
}

void Application::OnKeyDown(WPARAM wParam)
{
    return;
    //Client::Get().OnKeyDown(wParam);
}

void Application::OnKeyUp(WPARAM wParam)
{
    return;
    //Client::Get().OnKeyUp(wParam);
}

void Application::OnLButtonDown(LPARAM lParam)
{
    Client::Get().OnLButtonDown(lParam);
}

void Application::OnRButtonDown(LPARAM lParam)
{
    Client::Get().OnRButtonDown(lParam);
}
