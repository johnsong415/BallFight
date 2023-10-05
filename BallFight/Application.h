#pragma once

#include "framework.h"
#include "Messages.h"

class Application
{
public:
    Application();
    ~Application();

public:
    void Initialize(HWND hWnd, const std::string& ipAddress);
    void Shutdown();

    void Update();
    void ProcessCommands();
    
    static Application& Get();

    void StartServer();
    void StartClient();
    static void OnServerStarted(ErrorCode errorCode);
    static void OnClientStarted(ErrorCode errorCode);

    bool IsRunning() const;

public:
    void OnKeyDown(WPARAM wParam);
    void OnKeyUp(WPARAM wParam);
    void OnLButtonDown(LPARAM lParam);
    void OnRButtonDown(LPARAM lParam);

private:
    
    DWORD m_lastTickCount = 0;

    bool m_clientStarted = false;
    bool m_serverStarted = false;

    bool m_isRunning = false;
};
