#pragma once

#include "framework.h"


#define MONSTER_LIMIT       5
#define SEC_PER_MONSTER     3


class MonsterSpawnManager
{
public:
    MonsterSpawnManager();
    ~MonsterSpawnManager();

public:
    void StartGame();
    bool AddMonster();
    void DeleteMonster();


private:
    bool m_gameStarted = false;
    int m_numMonsters = 0;
    DWORD m_lastSpawn = GetTickCount();
};

