#include "World.h"
#include "Networking.h"
#include "Consts.h"

#include <unordered_set>

World::World()
{
}

World::~World()
{
}

void World::Update(int ms)
{
    return;
}

void World::AddObject(ObjectId id, Object* newObject)
{
    m_objects[id] = newObject;
    m_objectIds[newObject] = id;
}

void World::RemoveObject(ObjectId id)
{
    if (m_objects.find(id) != m_objects.end()) {
        Object* toRemove = m_objects[id];
        m_objects.erase(id);
        delete toRemove;
    }
}

void World::IterateAllObjects(ObjectIterateCallback&& callback)
{
    for (const auto& object : m_objects) {
        callback(object.second);
    }
}

Object* World::GetObject(ObjectId id)
{
    auto it = m_objects.find(id);
    if (it == m_objects.end()) return nullptr;

    return it->second;
}

ObjectId World::GetNewObjectId()
{
    ObjectId toReturn = m_nextObjectId;
    ++m_nextObjectId;
    return toReturn;
}

ObjectId World::GetLastId()
{
    return m_nextObjectId - 1;
}

ServerWorld::ServerWorld()
{

}

ServerWorld::~ServerWorld()
{

}

void ServerWorld::UpdatePlayer(Object* playerObject, int ms)
{
    assert(playerObject);

    Player* player = static_cast<Player*>(playerObject);
    player->UpdatePlayer(ms);
}

void ServerWorld::UpdateBall(Object* ballObject, int ms, std::unordered_set<ObjectId>& toRemove)
{
    assert(ballObject);

    Ball* ball = static_cast<Ball*>(ballObject);

    ball->Update(ms);

    bool shotByPlayer = ball->IsShotByPlayer();

    std::unordered_set<ObjectId> targets;

    if (shotByPlayer) {
        // target monsters and other player
        for (auto id : m_playerIds) {
            if (id != ball->GetShooter()) {
                targets.insert(id);
            }
        }
        for (auto id : m_monsterIds) {
            targets.insert(id);
        }
    }
    else {
        // target players only
        for (auto id : m_playerIds) {
            targets.insert(id);
        }
    }

    for (auto id : targets) {
        Object* target = GetObject(id);

        bool collided = ball->HasCollided(target);

        if (collided) {
            Object::ObjectType type = target->GetType();

            if (type == Object::ObjectType::MONSTER) {
                Monster* monster = static_cast<Monster*>(target);
                KillMonster* rpcMsg = new KillMonster;
                rpcMsg->monsterId = monster->GetId();
                toRemove.insert(monster->GetId());
                m_monsterSpawnManager.DeleteMonster();

                ServerMessage* killMonster = new ServerMessage(rpcMsg);
                Networking::Get().PostToComms(killMonster);
            }
            else if (type == Object::ObjectType::PLAYER) {
                Player* player = static_cast<Player*>(target);
                player->Die();
                GameOver* rpcMsg = new GameOver;
                rpcMsg->loser = player->GetId();
                rpcMsg->ball = ball->GetId();

                ServerMessage* gameOver = new ServerMessage(rpcMsg);
                Networking::Get().PostToComms(gameOver);
            }
        }
    }
    if (!ball->IsValid()) {
        --m_activeBalls[ball->GetShooter()];
        toRemove.insert(ball->GetId());
    }
}

void ServerWorld::UpdateMonster(Object* object, int ms, std::unordered_map<ObjectId, Object*>& toAdd)
{
    assert(object);

    Monster* monster = static_cast<Monster*>(object);
    monster->Update(ms);

    // collision detection with player
    std::unordered_set<ObjectId> targets;
    for (auto id : m_playerIds) {
        Player* player = static_cast<Player*>(GetObject(id));
        bool collided = monster->HasCollided(player);
        if (collided) {
            player->Die();
            GameOver* rpcMsg = new GameOver;
            rpcMsg->loser = player->GetId();
            rpcMsg->ball = monster->GetId();

            ServerMessage* addedPlayer = new ServerMessage(rpcMsg);
            Networking::Get().PostToComms(addedPlayer);
        }
    }



    bool canChangeDest = monster->TimeToChangeDirection(ms);
    if (canChangeDest) {
        monster->ChangeDirection();
        Vector2D newDest = monster->GetDestination();

        MoveMonsterTo* rpcMsg = new MoveMonsterTo(monster->GetId(), newDest.x, newDest.y);
        ServerMessage* changeDest = new ServerMessage(rpcMsg);
        Networking::Get().PostToComms(changeDest);
    }

    bool canShoot = monster->CanShoot(ms);
    if (canShoot) {
        ObjectId playerId = monster->GetNextPlayerToShoot();
        Player* player = static_cast<Player*>(GetObject(playerId));
        assert(player);

        Vector2D playerLocation = player->GetCoordinates();
        Vector2D monsterLocation = monster->GetCoordinates();
        Vector2D aim = monster->Aim(playerLocation);
        ObjectId ballId = GetNewObjectId();
        Ball* newBall = new Ball(ballId, monster->GetId(), monsterLocation, aim, false, COLOR_MONSTER_BALL);
        toAdd.insert({ ballId, newBall });

        monster->Shoot();

        ShootBall* rpcMsg = new ShootBall(COLOR_MONSTER_BALL, ballId, monsterLocation.x, monsterLocation.y, aim.x, aim.y);
        ServerMessage* changeDest = new ServerMessage(rpcMsg);
        Networking::Get().PostToComms(changeDest);
    }
}

void ServerWorld::Update(int ms)
{
    std::unordered_set<ObjectId> toRemove;
    std::unordered_map<ObjectId, Object*> toAdd;
    IterateAllObjects(
        [&](Object* object)
        {
            // TODO: unordered map with pointer to member
            switch (object->GetType()) {
            case Object::ObjectType::PLAYER:
                UpdatePlayer(object, ms);
                break;

            case Object::ObjectType::BALL:
                UpdateBall(object, ms, toRemove);
                break;

            case Object::ObjectType::MONSTER:
                UpdateMonster(object, ms, toAdd);
                break;

            }
        });

    for (auto objectId : toRemove) {
        RemoveObject(objectId);
    }

    bool canAddMonster = m_monsterSpawnManager.AddMonster();
    if (canAddMonster) {
        ObjectId monsterId = GetNewObjectId();
        m_monsterIds.insert(monsterId);
        Monster* newMonster = new Monster(monsterId);
        Vector2D monsterLocation = newMonster->GetCoordinates();
        Vector2D monsterDestination = newMonster->GetDestination();

        NewMonster* rpcMsg = new NewMonster(monsterId, monsterLocation.x, monsterLocation.y, monsterDestination.x, monsterDestination.y);
        ServerMessage* changeDest = new ServerMessage(rpcMsg);
        Networking::Get().PostToComms(changeDest);

        toAdd.insert({monsterId, newMonster});
    }

    for (auto [id, pointer] : toAdd) {
        AddObject(id, pointer);
    }
}

bool ServerWorld::CastBall(ObjectId playerId, float destX, float destY)
{
    Player* player = static_cast<Player*>(GetObject(playerId));
    if (!player || !player->IsValid()) return false;

    if (m_activeBalls[playerId] >= BALL_LIMIT) return false;

    CreateBall(playerId, destX, destY);
    return true;
}

void ServerWorld::CreateBall(ObjectId playerId, float destX, float destY)
{
    ++m_activeBalls[playerId];


    ObjectId id = GetNewObjectId();
    Ball* newBall = new Ball(id, playerId, GetObject(playerId)->GetCoordinates(), {destX, destY}, true);
    
    AddObject(id, newBall);
}

bool ServerWorld::SetDestination(ObjectId playerId, Vector2D destination)
{
    Player* player = static_cast<Player*>(GetObject(playerId));
    if (!player || !player->IsValid()) return false;

    player->SetDestination(destination);
    return true;
}

void ServerWorld::AddPlayer(ObjectId playerId, Vector2D location, Color color, Color ballColor)
{
    if (m_lobbyFull) return;

    Player* newPlayer = new Player(playerId, location, color, ballColor);
    AddObject(playerId, newPlayer);
    m_playerIds.insert(playerId);
    m_activeBalls[playerId] = 0;

    if (m_playerIds.size() == 2) {
        m_lobbyFull = true;
        m_monsterSpawnManager.StartGame();
    }
}

void ServerWorld::IteratePlayers(PlayerIterateCallback&& callback)
{
    for (const auto& id : m_playerIds) {
        callback(id, GetObject(id));
    }
}

Vector2D ServerWorld::GetPlayerCoords(ObjectId playerId)
{
    return GetObject(playerId)->GetCoordinates();
}

Player* ServerWorld::GetPlayer(ObjectId playerId)
{
    Player* player = static_cast<Player*>(GetObject(playerId));
    if (!player) return nullptr;
    
    return player;
}

void ServerWorld::RemovePlayer(ObjectId playerId)
{
    RemoveObject(playerId);
    m_playerIds.erase(playerId);
}

ClientWorld::ClientWorld()
{

}

ClientWorld::~ClientWorld()
{
}

ClientWorld& ClientWorld::Get()
{
    return *this;
}

void ClientWorld::MakeNewBall(Color color, ObjectId ballId, float locX, float locY, float destX, float destY)
{
    Ball* newBall = new Ball(ballId, INVALID_OBJECT_ID, { locX, locY }, { destX, destY }, false, color);
    AddObject(ballId, newBall);
}

void ClientWorld::AddPlayer(ObjectId playerId, float x, float y, Color color, Color ballColor)
{
    Player* newPlayer = new Player(playerId, {x, y}, color, ballColor);
    m_playerIds.insert(playerId);
    AddObject(playerId, newPlayer);
}

void ClientWorld::AddMonster(ObjectId monsterId, float x, float y, float dx, float dy)
{
    Monster* newMonster = new Monster(monsterId, { x, y }, { dx, dy }, COLOR_MONSTER);
    
    AddObject(monsterId, newMonster);
}

void ClientWorld::SetMonsterDestination(ObjectId monsterId, float x, float y)
{
    Monster* monster = static_cast<Monster*>(GetObject(monsterId));
    if (!monster) return;

    monster->SetDestination({ x, y });
}

void ClientWorld::KillMonster(ObjectId monsterId)
{
    RemoveObject(monsterId);
}

void ClientWorld::SetDestination(ObjectId playerId, Vector2D destination)
{
    Player* player = static_cast<Player*>(GetObject(playerId));
    if (!player || !player->IsValid()) return;

    player->SetDestination(destination);
}

void ClientWorld::KillPlayer(ObjectId playerId, ObjectId ballId)
{
    Player* player = static_cast<Player*>(GetObject(playerId));
    if (!player) return;
    player->Die();
    player->SetColor(COLOR_DEAD_OBJECT);

    Object* object = GetObject(ballId);
    if (!object) return;
    if (object->GetType() == Object::ObjectType::BALL) {
        Ball* ball = static_cast<Ball*>(object);
        if (!ball) return;
        ball->Explode();
    }    
}

bool ClientWorld::IsLocalPlayerAlive()
{
    Player* player = static_cast<Player*>(GetObject(m_localPlayerId));
    if (!player) return false;

    return player->IsValid();
}

void ClientWorld::UpdatePlayer(Object* playerObject, int ms)
{
    Player* player = static_cast<Player*>(playerObject);
    if (!player) return;

    player->UpdatePlayer(ms);
}

void ClientWorld::UpdateBall(Object* ball, int ms, std::unordered_set<ObjectId>& toRemove)
{
    // Client side will not check collisions
    Ball* pBall = static_cast<Ball*>(ball);
    pBall->Update(ms);
    if (!pBall->IsValid()) {
        toRemove.insert(pBall->GetId());
    }
}

void ClientWorld::UpdateMonster(Object* object, int ms)
{
    Monster* monster = static_cast<Monster*>(object);
    monster->Update(ms);
}

void ClientWorld::Update(int ms)
{
    std::unordered_set<ObjectId> toRemove;
    IterateAllObjects(
        [&](Object* object)
        {
            switch (object->GetType()) {
            case Object::ObjectType::PLAYER:
                UpdatePlayer(object, ms);
                break;

            case Object::ObjectType::BALL:
                UpdateBall(object, ms, toRemove);
                break;

            case Object::ObjectType::MONSTER:
                UpdateMonster(object, ms);
                break;
            }

        });
    
    for (auto objectId : toRemove) {
        RemoveObject(objectId);
    }
}

ObjectId ClientWorld::GetId()
{
    return m_localPlayerId;
}

void ClientWorld::SetId(ObjectId id)
{
    m_localPlayerId = id;
}

Player* ClientWorld::GetPlayer(ObjectId id)
{
    return static_cast<Player*>(GetObject(id));
}

