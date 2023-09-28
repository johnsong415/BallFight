#pragma once

#include "framework.h"
#include "Messages.h"

class Application
{
public:
    Application();
    ~Application();

public:
    void Initialize(HWND hWnd);

    void Update();
    void ProcessCommands();
    
    static Application& Get();

    void StartServer();
    void StartClient();
    static void OnServerStarted(ErrorCode errorCodeServer);
    static void OnClientStarted(ErrorCode errorCodeServer);

public:
    void OnKeyDown(WPARAM wParam);
    void OnKeyUp(WPARAM wParam);
    void OnLButtonDown(LPARAM lParam);
    void OnRButtonDown(LPARAM lParam);

private:
    
    DWORD m_lastTickCount = 0;

    bool m_started = false;

    bool m_client = false;
    bool m_server = false;
};
