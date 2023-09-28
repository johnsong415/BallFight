#include "Server.h"

#include "Networking.h"

#include "Messages.h"
#include "Consts.h"


Server g_server;


Server::Server()
{
    m_commandToProcess[MOVE_REQUEST] = new MoveRequestHandler();
    m_commandToProcess[SHOOT_REQUEST] = new ShootRequestHandler();
}

Server::~Server()
{
}

void Server::Update(int ms)
{
    ProcessMessage();
    UpdateWorld(ms);

}

Server& Server::Get()
{
    return g_server;
}

void Server::ProcessMessage()
{
    Message* received = Networking::Get().GetServerFromComms();
    if (received == nullptr) return;

    if (received->type != Message::CLIENT) {
        // TODO: log?
        OutputDebugString(L"Unexpected message type!\n");
        return;
    }

    ClientMessage* clientReceived = reinterpret_cast<ClientMessage*>(received);
    m_commandToProcess[clientReceived->message->command]->Handle(clientReceived, &m_world);
    
    delete clientReceived->message;
    delete clientReceived;
}

void Server::UpdateWorld(int ms)
{
    m_world.Update(ms);
}

void Server::AddPlayer(int playerId)
{
    Vector2D location;
    Color color;
    Color ballColor;
    // TODO: non random numbers
    if (playerId % 2) {
        location.x = 500;
        location.y = 500;
        ballColor = COLOR_PLAYER2_BALL;
        color = COLOR_PLAYER2;
    }
    else {
        location.x = 100;
        location.y = 100;
        ballColor = COLOR_PLAYER1_BALL;
        color = COLOR_PLAYER1;
    }

    m_world.AddPlayer(playerId, location, color, ballColor);
    m_world.IteratePlayers([](ObjectId id, const Object* object) {
        const Player* player = static_cast<const Player*>(object);
        NewPlayer* rpcMsg = new NewPlayer;
        rpcMsg->playerId = id;
        rpcMsg->x = player->GetCoordinates().x;
        rpcMsg->y = player->GetCoordinates().y;
        rpcMsg->playerColor = player->GetColor();
        rpcMsg->ballColor = player->GetBallColor();

        ServerMessage* addedPlayer = new ServerMessage(rpcMsg);
        Networking::Get().PostToComms(addedPlayer);
        });    
}
