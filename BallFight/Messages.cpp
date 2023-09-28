#include "Messages.h"


Message::Message(Type messageType) : type(messageType)
{}

InternalMessage::InternalMessage() : Message(Message::INTERNAL)
{}


RpcMessage::RpcMessage(RpcType rpcType, RpcCommand rpcCommand) : Message(Message::RPC), type(rpcType), command(rpcCommand)
{}

void RpcMessage::Serialize(PacketBuffer& buffer, int& size)
{}
void RpcMessage::Parse(PacketBuffer& buffer)
{}


ShootRequest::ShootRequest() : RpcMessage(CLIENTTOSERVER, SHOOT_REQUEST), x(0), y(0)
{}

ShootRequest::ShootRequest(float paramx, float paramy) : RpcMessage(CLIENTTOSERVER, SHOOT_REQUEST), x(paramx), y(paramy)
{}

void ShootRequest::Serialize(PacketBuffer& buffer, int& size)
{
    size = 0;
    size += sizeof(RpcCommand); // RPC Command
    size += sizeof(float); // float x
    size += sizeof(float); // float y
    buffer.Append(size);

    buffer.Append(SHOOT_REQUEST);

    buffer.Append(x);
    buffer.Append(y);
}

void ShootRequest::Parse(PacketBuffer& buffer)
{
    buffer.Read(x);
    buffer.Read(y);
}


ClientMessage::ClientMessage(ObjectId id, RpcMessage* msg) : Message(CLIENT), message(msg), playerId(id)
{}


ServerMessage::ServerMessage(RpcMessage* rpcMsg) : Message(SERVER), rpcMessage(rpcMsg)
{}


AssignID::AssignID(ObjectId id) : RpcMessage(SERVERTOCLIENT, ASSIGN_ID), playerId(id)
{}

void AssignID::Serialize(PacketBuffer& buffer, int& size)
{
    size = 0;
    size += sizeof(RpcCommand); // RPC Command
    size += sizeof(ObjectId); // player ID

    buffer.Append(size);
    buffer.Append(ASSIGN_ID);

    buffer.Append(playerId);
}

void AssignID::Parse(PacketBuffer& buffer)
{
    buffer.Read(playerId);
}


ShootBall::ShootBall() : RpcMessage(SERVERTOCLIENT, SHOOT_BALL)
{}

ShootBall::ShootBall(Color ballColor, ObjectId ballid, float ox, float oy, float dx, float dy) : RpcMessage(SERVERTOCLIENT, SHOOT_BALL), color(ballColor), id(ballid), orgX(ox), orgY(oy), destX(dx), destY(dy)
{}

void ShootBall::Serialize(PacketBuffer& buffer, int& size)
{
    size = 0;
    size += sizeof(RpcCommand); // RPC Command
    size += sizeof(ObjectId); // ball's id
    size += sizeof(Color); // ball color
    size += sizeof(float); // float origin x
    size += sizeof(float); // float origin y
    size += sizeof(float); // float destination x
    size += sizeof(float); // float destination y

    buffer.Append(size);
    buffer.Append(SHOOT_BALL);

    buffer.Append(color);
    buffer.Append(id);
    buffer.Append(orgX);
    buffer.Append(orgY);
    buffer.Append(destX);
    buffer.Append(destY);
}

void ShootBall::Parse(PacketBuffer& buffer)
{
    buffer.Read(color);
    buffer.Read(id);
    buffer.Read(orgX);
    buffer.Read(orgY);
    buffer.Read(destX);
    buffer.Read(destY);
}


NewPlayer::NewPlayer() : RpcMessage(SERVERTOCLIENT, NEW_PLAYER)
{}

NewPlayer::NewPlayer(ObjectId pId, float startX, float startY, Color color, Color bColor) : RpcMessage(SERVERTOCLIENT, NEW_PLAYER), playerId(pId), x(startX), y(startY), playerColor(color), ballColor(bColor)
{}

void NewPlayer::Serialize(PacketBuffer& buffer, int& size)
{
    size = 0;
    size += sizeof(RpcCommand); // RPC Command
    size += sizeof(ObjectId); // player id
    size += sizeof(float); // starting x
    size += sizeof(float); // starting y
    size += sizeof(Color); // player color
    size += sizeof(Color); // ball color

    buffer.Append(size);
    buffer.Append(NEW_PLAYER);

    buffer.Append(playerId);
    buffer.Append(x);
    buffer.Append(y);
    buffer.Append(playerColor);
    buffer.Append(ballColor);
}

void NewPlayer::Parse(PacketBuffer& buffer)
{
    buffer.Read(playerId);
    buffer.Read(x);
    buffer.Read(y);
    buffer.Read(playerColor);
    buffer.Read(ballColor);
}


MoveRequest::MoveRequest() : RpcMessage(CLIENTTOSERVER, MOVE_REQUEST)
{}

MoveRequest::MoveRequest(float x, float y) : RpcMessage(CLIENTTOSERVER, MOVE_REQUEST), destinationX(x), destinationY(y)
{}

void MoveRequest::Serialize(PacketBuffer& buffer, int& size)
{
    size = 0;
    size += sizeof(RpcCommand); // RPC Command
    size += sizeof(float); // destination x
    size += sizeof(float); // destination y
    buffer.Append(size);
    buffer.Append(MOVE_REQUEST);
    buffer.Append(destinationX);
    buffer.Append(destinationY);
}

void MoveRequest::Parse(PacketBuffer& buffer)
{
    buffer.Read(destinationX);
    buffer.Read(destinationY);
}


MoveTo::MoveTo() : RpcMessage(SERVERTOCLIENT, MOVE_TO)
{}

MoveTo::MoveTo(ObjectId pId, float x, float y) : RpcMessage(SERVERTOCLIENT, MOVE_TO), playerId(pId), destinationX(x), destinationY(y)
{}

void MoveTo::Serialize(PacketBuffer& buffer, int& size)
{
    size = 0;
    size += sizeof(RpcCommand); // RPC Command
    size += sizeof(ObjectId); // player id
    size += sizeof(float); // destination x
    size += sizeof(float); // destination y
    buffer.Append(size);
    buffer.Append(MOVE_TO);
    buffer.Append(playerId);
    buffer.Append(destinationX);
    buffer.Append(destinationY);
}

void MoveTo::Parse(PacketBuffer& buffer)
{
    buffer.Read(playerId);
    buffer.Read(destinationX);
    buffer.Read(destinationY);
}


MoveMonsterTo::MoveMonsterTo() : RpcMessage(SERVERTOCLIENT, MOVE_MONSTER_TO)
{}

MoveMonsterTo::MoveMonsterTo(ObjectId mId, float x, float y) : RpcMessage(SERVERTOCLIENT, MOVE_MONSTER_TO), monsterId(mId), destinationX(x), destinationY(y)
{}

void MoveMonsterTo::Serialize(PacketBuffer& buffer, int& size)
{
    size = 0;
    size += sizeof(RpcCommand); // RPC Command
    size += sizeof(ObjectId); // monster id
    size += sizeof(float); // destination x
    size += sizeof(float); // destination y
    buffer.Append(size);
    buffer.Append(MOVE_MONSTER_TO);
    buffer.Append(monsterId);
    buffer.Append(destinationX);
    buffer.Append(destinationY);
}

void MoveMonsterTo::Parse(PacketBuffer& buffer)
{
    buffer.Read(monsterId);
    buffer.Read(destinationX);
    buffer.Read(destinationY);
}


GameOver::GameOver() : RpcMessage(SERVERTOCLIENT, GAME_OVER)
{}

GameOver::GameOver(ObjectId lId, ObjectId bId) : RpcMessage(SERVERTOCLIENT, GAME_OVER), loser(lId), ball(bId)
{}

void GameOver::Serialize(PacketBuffer& buffer, int& size)
{
    size = 0;
    size += sizeof(RpcCommand); // RPC Command
    size += sizeof(ObjectId); // loser
    size += sizeof(ObjectId); // ball
    buffer.Append(size);
    buffer.Append(GAME_OVER);
    buffer.Append(loser);
    buffer.Append(ball);
}

void GameOver::Parse(PacketBuffer& buffer)
{
    buffer.Read(loser);
    buffer.Read(ball);
}


NewMonster::NewMonster() : RpcMessage(SERVERTOCLIENT, NEW_MONSTER)
{}

NewMonster::NewMonster(ObjectId oId, float startX, float startY, float destX, float destY) : RpcMessage(SERVERTOCLIENT, NEW_MONSTER), monsterId(oId), x(startX), y(startY), dx(destX), dy(destY)
{}

void NewMonster::Serialize(PacketBuffer& buffer, int& size)
{
    size = 0;
    size += sizeof(RpcCommand); // RPC Command
    size += sizeof(ObjectId); // player id
    size += sizeof(float); // starting x
    size += sizeof(float); // starting y
    size += sizeof(float); // dest x
    size += sizeof(float); // dest y

    buffer.Append(size);
    buffer.Append(NEW_MONSTER);

    buffer.Append(monsterId);
    buffer.Append(x);
    buffer.Append(y);
    buffer.Append(dx);
    buffer.Append(dy);
}

void NewMonster::Parse(PacketBuffer& buffer)
{
    buffer.Read(monsterId);
    buffer.Read(x);
    buffer.Read(y);
    buffer.Read(dx);
    buffer.Read(dy);
}

KillMonster::KillMonster() : RpcMessage(SERVERTOCLIENT, KILL_MONSTER)
{}

KillMonster::KillMonster(ObjectId id) : RpcMessage(SERVERTOCLIENT, KILL_MONSTER), monsterId(id)
{}

void KillMonster::Serialize(PacketBuffer& buffer, int& size)
{
    size = 0;
    size += sizeof(RpcCommand); // RPC Command
    size += sizeof(ObjectId); // monster id

    buffer.Append(size);
    buffer.Append(KILL_MONSTER);

    buffer.Append(monsterId);
}

void KillMonster::Parse(PacketBuffer& buffer)
{
    buffer.Read(monsterId);
}
