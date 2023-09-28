#include "Object.h"
#include "Graphics.h"

#include <cmath>
#include <cstdlib>


Object::Object(ObjectId id, ObjectType type, const Vector2D& coords, float speed, Color color)
    : m_coordinates(coords)
    , m_type(type)
    , m_color(color)
    , m_id(id)
    , m_speed(speed)
{
}

Object::~Object()
{

}

void Object::SetDestination(const Vector2D& newDestination)
{
    if (newDestination == m_destination) return;

    m_destination = newDestination;

    float diffX = newDestination.x - m_coordinates.x;
    float diffY = newDestination.y - m_coordinates.y;

    float diffLength = sqrt(diffX * diffX + diffY * diffY);

    if (diffLength == 0.f) return;

    m_velocity.x = m_speed * diffX / diffLength;
    m_velocity.y = m_speed * diffY / diffLength;
}

Vector2D Object::GetDestination()
{
    return m_destination;
}

const Vector2D& Object::GetCoordinates() const
{
    return m_coordinates;
}

Object::ObjectType Object::GetType() const
{
    return m_type;
}

void Object::SetColor(Color color)
{
    m_color = color;
}

Color Object::GetColor() const
{
    return m_color;
}

const ObjectId Object::GetId()
{
    return m_id;
}


Player::Player(ObjectId id, const Vector2D& coords, Color color, Color ballColor)
    : Object(id, Object::ObjectType::PLAYER, coords, PLAYER_SPEED, color), m_ballColor(ballColor)
{
}

Player::~Player()
{
}

void Player::Update(int ms)
{
    // vector representing distance and direction from current position to the destination
    Vector2D positionToDestination = m_destination - m_coordinates;
    
    // coordinates after this "step"
    Vector2D afterUpdate = m_coordinates + m_velocity * (float)ms;

    // vector representing distance and direction from current position to the destination
    Vector2D stepToDest = Vector2D(m_destination) - Vector2D(afterUpdate);
    
    // if dot product of vectors is less than 0 (i.e., it is negative, and we overstepped)
    if ((positionToDestination * stepToDest) <= 0) {
        m_coordinates = m_destination;
        m_velocity = {0.f, 0.f};
    }
    // otherwise, the dot product is positive and we keep going in this direction
    else {
        m_coordinates = afterUpdate;
    }
}

bool Player::IsValid()
{
    return m_alive;
}

void Player::Die()
{
    m_alive = false;
}

void Player::UpdatePlayer(int ms)
{
    Update(ms);
}

Color Player::GetBallColor() const
{
    return m_ballColor;
}


Ball::Ball(ObjectId id, ObjectId shooter, const Vector2D& location, const Vector2D& destination, bool shotByPlayer, Color color)
    : Object(id, Object::ObjectType::BALL, location, BALL_SPEED, color)
{
    SetDestination(destination);
    m_shooter = shooter;
    m_shotByPlayer = shotByPlayer;
}

Ball::~Ball()
{
}

void Ball::Update(int ms)
{
    m_coordinates = m_coordinates + m_velocity * (float)ms;
}

bool Ball::HasCollided(Object* opponent)
{
    if (opponent == nullptr) return false;
    float distX = GetCoordinates().x - opponent->GetCoordinates().x;
    float distY = GetCoordinates().y - opponent->GetCoordinates().y;

    float dist = sqrt(distX * distX + distY * distY);

    if (dist <= (BALL_CIRCLE_RADIUS + PLAYER_CIRCLE_RADIUS)) {
        m_alive = false;
        return true;
    }
    return false;
}

bool Ball::IsInScreen()
{
    float x = Object::GetCoordinates().x;
    float y = Object::GetCoordinates().y;

    return (x >= 0 && x <= SCREEN_WIDTH && y >= 0 && y <= SCREEN_HEIGHT);
}

ObjectId Ball::GetShooter()
{
    return m_shooter;
}

bool Ball::IsValid()
{
    if (!IsInScreen()) return false;
    return m_alive;
}

bool Ball::IsShotByPlayer()
{
    return m_shotByPlayer;
}

void Ball::Explode()
{
    m_alive = false;
}

Monster::Monster(ObjectId id, const Vector2D& location, const Vector2D& destination, Color color)
    : Object(id, Object::ObjectType::MONSTER, location, MONSTER_SPEED, color)
{
    m_coordinates = location;
    SetDestination(destination);
}

Monster::Monster(ObjectId id)
    : Object(id, Object::ObjectType::MONSTER, GetRandomCoordinates(), MONSTER_SPEED, COLOR_DEFAULT)
{
    SetDestination(GetRandomCoordinates());
}

Monster::~Monster()
{}

void Monster::Update(int ms)
{
    Vector2D positionToDestination = m_destination - m_coordinates;
    Vector2D afterUpdate = m_coordinates + m_velocity * (float)ms;
    Vector2D stepToDest = Vector2D(m_destination) - Vector2D(afterUpdate);
    if ((positionToDestination * stepToDest) <= 0) {
        m_coordinates = m_destination;
        m_velocity = { 0.f, 0.f };
    }
    else {
        m_coordinates = afterUpdate;
    }
}

bool Monster::IsValid()
{
    return m_alive;
}

bool Monster::CanShoot(int ms)
{
    if (m_timeElapsedSinceShot >= MONSTER_SHOOT_TIME) {
        return true;
    }
    else m_timeElapsedSinceShot += ms;
    return false;
}

ObjectId Monster::GetNextPlayerToShoot()
{
    return m_nextPlayerToShoot;
}

void Monster::Shoot()
{
    m_timeElapsedSinceShot = 0;
    if (m_nextPlayerToShoot == 0) m_nextPlayerToShoot = 1;
    else if (m_nextPlayerToShoot == 1) m_nextPlayerToShoot = 0;
}

Vector2D Monster::Aim(Vector2D destination)
{
    return {(destination.x - MONSTER_OFFSET + std::rand() % (2 * MONSTER_OFFSET)), (destination.y - MONSTER_OFFSET + std::rand() % (2 * MONSTER_OFFSET)) };
}

bool Monster::TimeToChangeDirection(int ms)
{
    if (m_timeElapsedSinceDirectionChanged >= MONSTER_REDIRECT_TIME) return true;
    else {
        m_timeElapsedSinceDirectionChanged += ms;
        return false;
    }
}

Vector2D Monster::GetRandomCoordinates()
{
    int x = std::rand() % SCREEN_WIDTH;
    int y = std::rand() % SCREEN_HEIGHT;

    return {(float)x, (float)y};
}

void Monster::ChangeDirection()
{
    m_timeElapsedSinceDirectionChanged = 0;
    SetDestination(GetRandomCoordinates());
}

bool Monster::HasCollided(Player* opponent)
{
    if (opponent == nullptr) return false;
    float distX = GetCoordinates().x - opponent->GetCoordinates().x;
    float distY = GetCoordinates().y - opponent->GetCoordinates().y;

    float dist = sqrt(distX * distX + distY * distY);

    if (dist <= (BALL_CIRCLE_RADIUS + PLAYER_CIRCLE_RADIUS)) {
        m_alive = false;
        return true;
    }
    return false;
}
