#pragma once


#include <functional>

struct Vector2D
{
    float x = 0.f;
    float y = 0.f;

    Vector2D();
    Vector2D(float argx, float argy);

    float operator*(const Vector2D& v);
    Vector2D operator*(float v);
    Vector2D operator+(const Vector2D& v);
    Vector2D operator-(const Vector2D& v);
    bool operator==(const Vector2D& v) const;
};


class ScopeExit
{
public:
    using ScopeExitCallback = std::function<void()>;

    ScopeExit(ScopeExitCallback&& callback)
    {
        m_callback = callback;
    }

    ~ScopeExit()
    {
        if (m_callback) m_callback();
    }

    void Release()
    {
        m_callback = nullptr;
    }

private:
    ScopeExitCallback m_callback;
};
