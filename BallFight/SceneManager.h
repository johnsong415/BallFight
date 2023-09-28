#pragma once

#include "Scenes.h"

#include <unordered_map>


class SceneManager
{
public:
	SceneManager();
	~SceneManager();

public:
    static void Update();
	static SceneManager& Get();

private:
    void RenderCurrentScene();

public:
	enum class SceneType {
		FRONTEND,
		INGAME,
        GAMEOVER
	};

public:
	void SwitchScene(SceneType newScene);
	SceneType CurrentScene();

private:
	std::unordered_map<SceneType, Scene*> m_scenes;
    SceneType m_currentSceneType = SceneType::FRONTEND;
};
