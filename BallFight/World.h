#pragma once

#include "Object.h"

#include "Messages.h"

#include "MonsterSpawnManager.h"

#include <unordered_set>

#include <functional>

#include <unordered_map>




using ObjectIterateCallback = std::function<void(Object * object)>;
using PlayerIterateCallback = std::function<void(ObjectId id, const Object* object)>;


class World
{
public:
    World();
    ~World();

public:
    virtual void Update(int ms);
    virtual void UpdatePlayer(Object* player, int ms) = 0;
    virtual void UpdateBall(Object* ball, int ms, std::unordered_set<ObjectId>& toRemove) = 0;
    //virtual void UpdateMonster(Object* object, int ms) = 0;

    void AddObject(ObjectId id, Object* newObject);
    void RemoveObject(ObjectId id);

    void IterateAllObjects(ObjectIterateCallback&& callback);

    ObjectId GetLastId();
protected:
    Object* GetObject(ObjectId id);

    //ObjectId GetObjectId(Object* pObject);
    ObjectId GetNewObjectId();

private:
    std::unordered_map<ObjectId, Object*> m_objects;
    std::unordered_map<Object*, ObjectId> m_objectIds; // keyed by Object*

    ObjectId m_nextObjectId = 2; // 0 and 1 reserved for players
};


#define BALL_LIMIT      5

class ServerWorld : public World
{
public:
    ServerWorld();
    ~ServerWorld();

public:
    void Update(int ms) override;
    void UpdatePlayer(Object* player, int ms) override;
    void UpdateBall(Object* ball, int ms, std::unordered_set<ObjectId>& toRemove) override;
    void UpdateMonster(Object* object, int ms, std::unordered_map<ObjectId, Object*>& toAdd);

    bool SetDestination(ObjectId playerId, Vector2D destination);

    bool CastBall(ObjectId playerId, float destX, float destY);

    void CreateBall(ObjectId playerId, float destX, float destY);

    void IteratePlayers(PlayerIterateCallback&& callback);

    Vector2D GetPlayerCoords(ObjectId playerId);

    void AddPlayer(ObjectId playerId, Vector2D location, Color color, Color ballColor);
    Player* GetPlayer(ObjectId playerId);
    void RemovePlayer(ObjectId playerId);


    // TODO: clean up after game over

private:
    MonsterSpawnManager m_monsterSpawnManager;

    std::unordered_map<ObjectId, int> m_activeBalls;
    std::unordered_set<ObjectId> m_playerIds; // playerIds
    std::unordered_set<ObjectId> m_monsterIds;

    bool m_lobbyFull = false;



};

class ClientWorld : public World
{
public:
    ClientWorld();
    ~ClientWorld();

public:
    ClientWorld& Get();

public:
    void Update(int ms) override;
    void UpdatePlayer(Object* player, int ms) override;
    void UpdateBall(Object* ball, int ms, std::unordered_set<ObjectId>& toRemove) override;
    void UpdateMonster(Object* object, int ms);

    void MakeNewBall(Color color, ObjectId ballId, float locX, float locY, float destX, float destY);
    
    void AddMonster(ObjectId monsterId, float x, float y, float dx, float dy); 
    void SetMonsterDestination(ObjectId monsterId, float x, float y);
    void KillMonster(ObjectId monsterId);
    
    void AddPlayer(ObjectId playerId, float x, float y, Color color, Color ballColor);
    void SetDestination(ObjectId playerId, Vector2D destination);
    void KillPlayer(ObjectId playerId, ObjectId ballId);
    bool IsLocalPlayerAlive();

    ObjectId GetId();
    void SetId(ObjectId id);

    Player* GetPlayer(ObjectId id);

private:
    //std::unordered_map<PlayerId, ObjectId> m_objectIds;
    std::unordered_set<ObjectId> m_playerIds;
    ObjectId m_localPlayerId = -1; // this client's id
};
