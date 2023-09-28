#pragma once

#include "Graphics.h"


class Scene
{
public:
	Scene();
	virtual ~Scene();

public:
    virtual void Render() = 0;
};


class SceneFrontend : public Scene
{
public:
    SceneFrontend();
    virtual ~SceneFrontend();

public:
    void Render() override;

private:
    Vector2D m_InsertCoinTextLocation;
};


class SceneInGame : public Scene
{
public:
    SceneInGame();
    virtual ~SceneInGame();

public:
    void Render() override;

private:
    Vector2D m_GameStartedTextLocation;
};

class SceneGameOver : public Scene
{
public:
    SceneGameOver();
    virtual ~SceneGameOver();

public:
    void Render() override;

private:
    Vector2D m_GameOverTextLocation;
};
