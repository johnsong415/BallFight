#pragma once

#include <unordered_map>


#include "World.h"
#include "RpcMessageHandler.h"
#include "MonsterSpawnManager.h"


class Server
{
public:
    Server();
    ~Server();

public:
    static Server& Get();

public:
    void Update(int ms);

    void ProcessMessage();
    void UpdateWorld(int ms);

    void AddPlayer(int playerId);
    void AddMonster();

private:
    ServerWorld m_world;
    //MonsterSpawnManager m_monsterSpawnManager;

    std::unordered_map<RpcCommand, RpcMessageHandler*> m_commandToProcess;

};