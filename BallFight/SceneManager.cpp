#include "SceneManager.h"


static SceneManager g_sceneManager;


void SceneManager::Update()
{
    g_sceneManager.RenderCurrentScene();
}

SceneManager& SceneManager::Get()
{
	return g_sceneManager;
}

SceneManager::SceneManager()
{
	m_scenes[SceneType::FRONTEND] = new SceneFrontend();
    m_scenes[SceneType::INGAME] = new SceneInGame();
    m_scenes[SceneType::GAMEOVER] = new SceneGameOver();
}

SceneManager::~SceneManager()
{
    // TODO: clean up
    for (auto& scene : m_scenes) {
        delete scene.second;
    }
}

void SceneManager::SwitchScene(SceneType newScene)
{
	m_currentSceneType = newScene;
}

SceneManager::SceneType SceneManager::CurrentScene()
{
	return m_currentSceneType;
}

void SceneManager::RenderCurrentScene()
{
    m_scenes[m_currentSceneType]->Render();
}
