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

void Application::Initialize(HWND hWnd, const std::string& ipAddress)
{
    Networking::Get().Initialize(ipAddress);
    Graphics::Initialize(hWnd);

    m_lastTickCount = GetTickCount();
    m_isRunning = true;
}

void Application::Update()
{
    // TODO: change QueryPerformanceCounter
    DWORD currentTickCount = GetTickCount();
    if (currentTickCount == m_lastTickCount) return;

    ProcessCommands();
    if (m_serverStarted) Server::Get().Update(currentTickCount - m_lastTickCount);
    if (m_clientStarted) Client::Get().Update(currentTickCount - m_lastTickCount);
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
    if (m_serverStarted || m_clientStarted) return;
    m_serverStarted = true;
    InternalMessage* startServer = new InternalMessage;
    startServer->command = START_SERVER;
    startServer->callback = OnServerStarted;
    Networking::Get().PostToComms(startServer);

}

void Application::StartClient()
{
    if (m_clientStarted) return;
    m_clientStarted = true;

    SceneManager::Get().SwitchScene(SceneManager::SceneType::INGAME);

    InternalMessage* startClient = new InternalMessage;
    startClient->command = START_CLIENT;
    startClient->callback = OnClientStarted;

    Networking::Get().PostToComms(startClient);
}

void Application::OnServerStarted(ErrorCode errorCode)
{
    if (errorCode != 0) {
        g_application.m_serverStarted = false;
        MessageBox(NULL, L"Server start failed. Please try again.", L"Server not started", MB_OK);
        return;
    }

    g_application.StartClient();
}

void Application::OnClientStarted(ErrorCode errorCode)
{
    if (errorCode != 0) {
        g_application.m_clientStarted = false;
        MessageBox(NULL, L"Client start failed. Please try again.", L"Client not started", MB_OK);
    }
}

void Application::OnKeyDown(WPARAM wParam)
{
    return;
}

void Application::OnKeyUp(WPARAM wParam)
{
    return;
}

void Application::OnLButtonDown(LPARAM lParam)
{
    Client::Get().OnLButtonDown(lParam);
}

void Application::OnRButtonDown(LPARAM lParam)
{
    Client::Get().OnRButtonDown(lParam);
}

void Application::Shutdown()
{
    m_isRunning = false;
}

bool Application::IsRunning() const
{
    return m_isRunning;
}

