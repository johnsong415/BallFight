#pragma once

#include "Messages.h"
#include "World.h"


class RpcMessageHandler
{
public:
    RpcMessageHandler();
    ~RpcMessageHandler();

public:
    virtual void Handle(Message* message, World* world) = 0;
};

class MoveRequestHandler : public RpcMessageHandler
{
public:
    MoveRequestHandler();
    ~MoveRequestHandler();

public:
    void Handle(Message* message, World* world) override;
};

class MoveToHandler : public RpcMessageHandler
{
public:
    MoveToHandler();
    ~MoveToHandler();

public:
    void Handle(Message* message, World* world) override;
};

class ShootRequestHandler : public RpcMessageHandler
{
public:
    ShootRequestHandler();
    ~ShootRequestHandler();

public:
    void Handle(Message* message, World* world) override;
};

class ShootBallHandler : public RpcMessageHandler
{
public:
    ShootBallHandler();
    ~ShootBallHandler();

public:
    void Handle(Message* message, World* world) override;
};

class NewPlayerHandler : public RpcMessageHandler
{
public:
    NewPlayerHandler();
    ~NewPlayerHandler();

public:
    void Handle(Message* message, World* world) override;
};

class NewMonsterHandler : public RpcMessageHandler
{
public:
    NewMonsterHandler();
    ~NewMonsterHandler();

public:
    void Handle(Message* message, World* world) override;
};

class MoveMonsterToHandler : public RpcMessageHandler
{
public:
    MoveMonsterToHandler();
    ~MoveMonsterToHandler();

public:
    void Handle(Message* message, World* world);
};

class KillMonsterHandler : public RpcMessageHandler
{
public:
    KillMonsterHandler();
    ~KillMonsterHandler();

public:
    void Handle(Message* message, World* world) override;
};

class AssignIdHandler : public RpcMessageHandler
{
public:
    AssignIdHandler();
    ~AssignIdHandler();

public:
    void Handle(Message* message, World* world) override;
};

class GameOverHandler : public RpcMessageHandler
{
public:
    GameOverHandler();
    ~GameOverHandler();

public:
    void Handle(Message* message, World* world) override;
};

