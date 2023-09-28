#include "MonsterSpawnManager.h"


MonsterSpawnManager::MonsterSpawnManager()
{}

MonsterSpawnManager::~MonsterSpawnManager()
{}

void MonsterSpawnManager::StartGame()
{
    m_gameStarted = true;
    m_lastSpawn = GetTickCount();
}

bool MonsterSpawnManager::AddMonster()
{
    if (!m_gameStarted) return false;

    DWORD currentTick = GetTickCount();
    if (m_numMonsters >= MONSTER_LIMIT || (currentTick - m_lastSpawn) < (SEC_PER_MONSTER * 1000)) return false;
    m_lastSpawn = currentTick;
    ++m_numMonsters;
    return true;
}

void MonsterSpawnManager::DeleteMonster()
{
    --m_numMonsters;
}
