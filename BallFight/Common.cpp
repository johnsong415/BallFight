#include "Common.h"


Vector2D::Vector2D()
{
}

Vector2D::Vector2D(float argx, float argy)
    : x(argx), y(argy)
{
}

float Vector2D::operator*(const Vector2D& v)
{
    return x * v.x + y * v.y;
}

Vector2D Vector2D::operator*(float v)
{
    return Vector2D(x * v, y * v);
}

Vector2D Vector2D::operator+(const Vector2D& v)
{
    Vector2D sum;
    sum.x = x + v.x;
    sum.y = y + v.y;

    return sum;
}

Vector2D Vector2D::operator-(const Vector2D& v)
{
    Vector2D sum;
    sum.x = x - v.x;
    sum.y = y - v.y;

    return sum;
}

bool Vector2D::operator==(const Vector2D& v) const
{
    return (x == v.x) && (y == v.y);
}
