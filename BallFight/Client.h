#pragma once

#include "World.h"

#include "RpcMessageHandler.h"

#include <unordered_map>


class Client
{
public:
    Client();
    ~Client();

public:
    static Client& Get();

public:
    void Update(int ms);
    void UpdateWorld(int ms);
    void ProcessMessage();
    //void OnKeyDown(WPARAM wParam);
    //void OnKeyUp(WPARAM wParam);
    void OnLButtonDown(LPARAM lParam);
    void OnRButtonDown(LPARAM lParam);

public:
    ClientWorld& GetWorld();

private:
    ClientWorld m_world;
    std::unordered_map<RpcCommand, RpcMessageHandler*> m_commandHandlers;
};
