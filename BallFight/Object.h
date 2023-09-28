#pragma once

#include "framework.h"


class Object
{
public:
    enum class ObjectType
    {
        PLAYER,
        BALL,
        MONSTER
    };

public:
	Object(ObjectId id, ObjectType type, const Vector2D& coords, float speed, Color color);
	virtual ~Object();

public:
	virtual void Update(int ms) = 0;
    virtual bool IsValid() = 0;
    void SetColor(Color color);

public:
    void SetDestination(const Vector2D& newDestination);
    Vector2D GetDestination();
    const Vector2D& GetCoordinates() const;
    const ObjectId GetId();

    ObjectType GetType() const;
    Color GetColor() const;

protected:
    Vector2D m_coordinates;
    Vector2D m_velocity;
    Vector2D m_destination;    

private:
    ObjectId m_id = INVALID_OBJECT_ID;
    ObjectType m_type;
    Color m_color;

    float m_speed = 0.f;
};

class Player : public Object
{
public:
    Player(ObjectId id, const Vector2D& coords, Color playerColor, Color ballColor);
    virtual ~Player();

public:
    void Update(int ms) override;
    void UpdatePlayer(int ms);
    
    Color GetBallColor() const;

    bool IsValid() override;
    // TODO: make destination current position
    void Die();

private:
    bool m_alive = true;
    
    Color m_ballColor;
};

class Ball : public Object
{
public:
    Ball(ObjectId id, ObjectId shooterId, const Vector2D& location, const Vector2D& destination, bool shotByPlayer, Color color = COLOR_DEFAULT);
    virtual ~Ball();

public:
    void Update(int ms) override;
    bool IsValid() override;
    bool IsInScreen();
    bool IsShotByPlayer();
    ObjectId GetShooter();

    void Explode();
    bool HasCollided(Object* opponent);

private:
    Vector2D m_dest;
    ObjectId m_shooter;

    bool m_alive = true;
    bool m_shotByPlayer = true;
};

class Monster : public Object
{
public:
    Monster(ObjectId id);
    Monster(ObjectId id, const Vector2D& location, const Vector2D& destination, Color color);
    virtual ~Monster();

public:
    void Update(int ms) override;
    bool IsValid() override;

    void ChangeDirection();

    ObjectId GetNextPlayerToShoot();

    bool CanShoot(int ms);
    Vector2D Aim(Vector2D destination);
    void Shoot();
    bool TimeToChangeDirection(int ms);
    bool HasCollided(Player* opponent);

private:
    Vector2D GetRandomCoordinates();

private:
    ObjectId m_nextPlayerToShoot = 0;
    Vector2D m_destination;

    bool m_alive = true;
    int m_timeElapsedSinceDirectionChanged = 0;
    int m_timeElapsedSinceShot = 0;
};
