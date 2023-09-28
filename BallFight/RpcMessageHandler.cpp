#include "RpcMessageHandler.h"


#include "Networking.h"
#include "SceneManager.h"


RpcMessageHandler::RpcMessageHandler()
{}

RpcMessageHandler::~RpcMessageHandler()
{}


MoveRequestHandler::MoveRequestHandler()
{}

MoveRequestHandler::~MoveRequestHandler()
{}

void MoveRequestHandler::Handle(Message* message, World* world)
{
    // This function is called in server
    ServerWorld* serverWorld = static_cast<ServerWorld*>(world);
    // The message is sent from a client
    ClientMessage* clientMessage = static_cast<ClientMessage*>(message);

    Player* player = serverWorld->GetPlayer(clientMessage->playerId);
    if (!player) return;


    MoveRequest* rpcSetDestRequest = static_cast<MoveRequest*>(clientMessage->message);
    Vector2D destination;
    destination.x = rpcSetDestRequest->destinationX;
    destination.y = rpcSetDestRequest->destinationY;

    bool succeeded = serverWorld->SetDestination(
        clientMessage->playerId,
        {rpcSetDestRequest->destinationX, rpcSetDestRequest->destinationY});
    if (!succeeded) return;

    RpcMessage* rpcDirChangeMsg = new MoveTo(clientMessage->playerId, rpcSetDestRequest->destinationX, rpcSetDestRequest->destinationY);
    ServerMessage* dirChangeMsg = new ServerMessage(rpcDirChangeMsg);
    Networking::Get().PostToComms(dirChangeMsg);
}


MoveToHandler::MoveToHandler()
{}

MoveToHandler::~MoveToHandler()
{}

void MoveToHandler::Handle(Message* message, World* world)
{
    // This function is called in client
    ClientWorld* clientWorld = static_cast<ClientWorld*>(world);
    // The message is sent from a server
    ServerMessage* serverMessage = static_cast<ServerMessage*>(message);
    MoveTo* setDest = static_cast<MoveTo*>(serverMessage->rpcMessage);

    Player* player = clientWorld->GetPlayer(setDest->playerId);
    if (!player) return;

    clientWorld->SetDestination(setDest->playerId, { setDest->destinationX, setDest->destinationY});
}


ShootRequestHandler::ShootRequestHandler()
{}

ShootRequestHandler::~ShootRequestHandler()
{}

void ShootRequestHandler::Handle(Message* message, World* world)
{
    // This function is called in server
    ServerWorld* serverWorld = static_cast<ServerWorld*>(world);
    // The message is sent from a client
    ClientMessage* clientMessage = static_cast<ClientMessage*>(message);

    Player* player = serverWorld->GetPlayer(clientMessage->playerId);
    if (!player) return;


    ShootRequest* rpcShootRequest = reinterpret_cast<ShootRequest*>(clientMessage->message);

    bool succeeded = serverWorld->CastBall(clientMessage->playerId, rpcShootRequest->x, rpcShootRequest->y);
    if (!succeeded) return;

    ShootBall* shootBall = new ShootBall(
        player->GetBallColor(),
        serverWorld->GetLastId(),
        serverWorld->GetPlayerCoords(clientMessage->playerId).x,
        serverWorld->GetPlayerCoords(clientMessage->playerId).y,
        rpcShootRequest->x,
        rpcShootRequest->y);

    Networking::Get().PostToComms(new ServerMessage(shootBall));
}

ShootBallHandler::ShootBallHandler()
{}

ShootBallHandler::~ShootBallHandler()
{}

void ShootBallHandler::Handle(Message* message, World* world)
{
    // This function is called in client
    ClientWorld* clientWorld = static_cast<ClientWorld*>(world);
    // The message is sent from a server
    ServerMessage* sMessage = static_cast<ServerMessage*>(message);

    ShootBall* shootBall = static_cast<ShootBall*>(sMessage->rpcMessage);
    clientWorld->MakeNewBall(shootBall->color, shootBall->id, shootBall->orgX, shootBall->orgY, shootBall->destX, shootBall->destY);
}


NewPlayerHandler::NewPlayerHandler()
{}

NewPlayerHandler::~NewPlayerHandler()
{}

void NewPlayerHandler::Handle(Message* message, World* world)
{
    // This function is called in client
    ClientWorld* clientWorld = static_cast<ClientWorld*>(world);
    // The message is sent from a server
    ServerMessage* sMessage = static_cast<ServerMessage*>(message);

    NewPlayer* newPlayer = static_cast<NewPlayer*>(sMessage->rpcMessage);
    clientWorld->AddPlayer(newPlayer->playerId, newPlayer->x, newPlayer->y, newPlayer->playerColor, newPlayer->ballColor);
}

NewMonsterHandler::NewMonsterHandler()
{}

NewMonsterHandler::~NewMonsterHandler()
{}

void NewMonsterHandler::Handle(Message* message, World* world)
{
    // This function is called in client
    ClientWorld* clientWorld = static_cast<ClientWorld*>(world);
    // The message is sent from a server
    ServerMessage* sMessage = static_cast<ServerMessage*>(message);

    NewMonster* newMonster = static_cast<NewMonster*>(sMessage->rpcMessage);
    clientWorld->AddMonster(newMonster->monsterId, newMonster->x, newMonster->y, newMonster->dx, newMonster->dy);
}


MoveMonsterToHandler::MoveMonsterToHandler()
{}

MoveMonsterToHandler::~MoveMonsterToHandler()
{}

void MoveMonsterToHandler::Handle(Message* message, World* world)
{
    // This function is called in client
    ClientWorld* clientWorld = static_cast<ClientWorld*>(world);
    // The message is sent from a server
    ServerMessage* sMessage = static_cast<ServerMessage*>(message);

    MoveMonsterTo* moveMonster = static_cast<MoveMonsterTo*>(sMessage->rpcMessage);
    clientWorld->SetMonsterDestination(moveMonster->monsterId, moveMonster->destinationX, moveMonster->destinationY);
}


KillMonsterHandler::KillMonsterHandler()
{}

KillMonsterHandler::~KillMonsterHandler()
{}

void KillMonsterHandler::Handle(Message* message, World* world)
{
    // This function is called in client
    ClientWorld* clientWorld = static_cast<ClientWorld*>(world);
    // The message is sent from a server
    ServerMessage* sMessage = static_cast<ServerMessage*>(message);

    KillMonster* killMonster = static_cast<KillMonster*>(sMessage->rpcMessage);
    clientWorld->KillMonster(killMonster->monsterId);
}


AssignIdHandler::AssignIdHandler()
{}

AssignIdHandler::~AssignIdHandler()
{}

void AssignIdHandler::Handle(Message* message, World* world)
{
    // This function is called in client
    ClientWorld* clientWorld = static_cast<ClientWorld*>(world);
    // The message is sent from a server
    ServerMessage* sMessage = static_cast<ServerMessage*>(message);

    AssignID* assignID = static_cast<AssignID*>(sMessage->rpcMessage);
    clientWorld->SetId(assignID->playerId);
}


GameOverHandler::GameOverHandler()
{}
    
GameOverHandler::~GameOverHandler()
{}
    
void GameOverHandler::Handle(Message* message, World* world)
{
    // This function is called in client
    ClientWorld* clientWorld = static_cast<ClientWorld*>(world);
    // The message is sent from a server
    ServerMessage* sMessage = static_cast<ServerMessage*>(message);

    GameOver* gameOver = static_cast<GameOver*>(sMessage->rpcMessage);
    clientWorld->KillPlayer(gameOver->loser, gameOver->ball);
    SceneManager::Get().SwitchScene(SceneManager::SceneType::GAMEOVER);
}

