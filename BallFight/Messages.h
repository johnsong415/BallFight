#pragma once

#include <functional>

#include "PacketBuffer.h"

#include "CommonTypes.h"


enum InternalCommand
{
    START_SERVER = 0,
    START_SERVER_ACK,
    START_CLIENT,
    START_CLIENT_ACK,
    STOP_SERVER,
    STOP_SERVER_ACK,
    STOP_CLIENT,
    CLIENT_ACCEPTED,
};

enum RpcCommand
{
    MOVE_REQUEST,
    MOVE_TO,
    MOVE_MONSTER_TO,
    SHOOT_REQUEST,
    SHOOT_BALL,
    NEW_PLAYER,
    ASSIGN_ID,
    GAME_OVER,
    NEW_MONSTER,
    KILL_MONSTER,
};


using ObjectId = int;
using ErrorCode = int;
using ErrorCodeCallback = std::function<void(int)>;



struct Message
{
    enum Type { INTERNAL, RPC, CLIENT, SERVER };

    Type type;

    Message(Type messageType);
};

struct InternalMessage : public Message
{
    InternalCommand command; // StartServer, StartClient, StopServer, etc.
    ErrorCodeCallback callback;
    int flag = 0; // used for playerID

    InternalMessage();
};

struct RpcMessage : public Message
{
    enum RpcType { SERVERTOCLIENT, CLIENTTOSERVER };

    RpcType type; // ServerToClient, ClientToServer
    RpcCommand command;

    RpcMessage(RpcType rpcType, RpcCommand rpcCommand);

    virtual void Serialize(PacketBuffer& buffer, int& size);
    virtual void Parse(PacketBuffer& buffer);
};

struct ShootRequest : public RpcMessage
{
    float x;
    float y;

    ShootRequest();
    ShootRequest(float paramx, float paramy);

    void Serialize(PacketBuffer& buffer, int& size);
    void Parse(PacketBuffer& buffer);
};

// Message *from* client
struct ClientMessage : public Message
{
    ObjectId playerId; // from this playerid
    RpcMessage* message;

    ClientMessage(ObjectId id, RpcMessage* msg);
};
// Message *from* server
struct ServerMessage : public Message
{
    RpcMessage* rpcMessage;

    ServerMessage(RpcMessage* rpcMsg);
};

struct AssignID : public RpcMessage
{
    ObjectId playerId;

    AssignID(ObjectId id);

    void Serialize(PacketBuffer& buffer, int& size);
    void Parse(PacketBuffer& buffer);
};

struct ShootBall : public RpcMessage
{
    Color color;
    ObjectId id;
    float orgX;
    float orgY;
    float destX;
    float destY;

    ShootBall();
    ShootBall(Color ballColor, ObjectId ballid, float ox, float oy, float dx, float dy);

    void Serialize(PacketBuffer& buffer, int& size);
    void Parse(PacketBuffer& buffer);
};

struct NewPlayer : public RpcMessage
{
    ObjectId playerId;
    float x;
    float y;
    Color playerColor;
    Color ballColor;

    NewPlayer();
    NewPlayer(ObjectId pId, float startX, float startY, Color color, Color bColor);

    void Serialize(PacketBuffer& buffer, int& size);
    void Parse(PacketBuffer& buffer);
};

struct MoveRequest : public RpcMessage
{
    float destinationX;
    float destinationY;

    MoveRequest();
    MoveRequest(float x, float y);

    void Serialize(PacketBuffer& buffer, int& size);
    void Parse(PacketBuffer& buffer);
};

struct MoveTo : public RpcMessage
{
    ObjectId playerId;
    float destinationX;
    float destinationY;

    MoveTo();
    MoveTo(ObjectId pId, float x, float y);

    void Serialize(PacketBuffer& buffer, int& size);
    void Parse(PacketBuffer& buffer);
};

struct MoveMonsterTo : public RpcMessage
{
    ObjectId monsterId;
    float destinationX;
    float destinationY;

    MoveMonsterTo();
    MoveMonsterTo(ObjectId pId, float x, float y);

    void Serialize(PacketBuffer& buffer, int& size);
    void Parse(PacketBuffer& buffer);
};

struct GameOver : public RpcMessage
{
    ObjectId loser;
    ObjectId ball;

    GameOver();
    GameOver(ObjectId lId, ObjectId bId);

    void Serialize(PacketBuffer& buffer, int& size);
    void Parse(PacketBuffer& buffer);
};

struct NewMonster : public RpcMessage
{
    ObjectId monsterId;
    float x;
    float y;
    float dx;
    float dy;

    NewMonster();
    NewMonster(ObjectId oId, float startX, float startY, float destX, float destY);

    void Serialize(PacketBuffer& buffer, int& size);
    void Parse(PacketBuffer& buffer);
};

struct KillMonster : public RpcMessage
{
    ObjectId monsterId;

    KillMonster();
    KillMonster(ObjectId id);

    void Serialize(PacketBuffer& buffer, int& size);
    void Parse(PacketBuffer& buffer);
};
