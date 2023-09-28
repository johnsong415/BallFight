#include "framework.h"
#include "Scenes.h"

#include "Client.h"
#include "World.h"

Scene::Scene()
{
}

Scene::~Scene()
{
}


SceneFrontend::SceneFrontend()
{
	// TODO: non magic numbers
	m_InsertCoinTextLocation.x = 100.f;
    m_InsertCoinTextLocation.y = 100.f;
}

SceneFrontend::~SceneFrontend()
{
}

void SceneFrontend::Render()
{
    Graphics::Get().Clear();
    Graphics::Get().DrawText(m_InsertCoinTextLocation, L"INSERT COIN", MEDIUM_FONT_SIZE);

    // remove below later
    Client::Get().GetWorld().IterateAllObjects(
        [](const Object* object) {
            switch (object->GetType()) {
            case Object::ObjectType::PLAYER:
                Graphics::Get().DrawCircle(PLAYER_CIRCLE_RADIUS, object->GetCoordinates(), object->GetColor());
                break;

            case Object::ObjectType::BALL:
                // draw circle
                Graphics::Get().DrawCircle(BALL_CIRCLE_RADIUS, object->GetCoordinates(), object->GetColor());
                break;

            case Object::ObjectType::MONSTER:
                // draw triangle
                Graphics::Get().DrawCircle(MONSTER_CIRCLE_RADIUS, object->GetCoordinates(), object->GetColor());
                break;
            }
        }
    );

    Graphics::Get().EndFrame();
}

SceneInGame::SceneInGame()
{
	// TODO: non magic numbers
	m_GameStartedTextLocation.x = 0.f;
    m_GameStartedTextLocation.y = 0.f;
}

SceneInGame::~SceneInGame()
{
}

void SceneInGame::Render()
{
    Graphics::Get().Clear();
    Graphics::Get().DrawText(m_GameStartedTextLocation, L"STARTED GAME", SMALL_FONT_SIZE);

    Client::Get().GetWorld().IterateAllObjects(
        [](const Object* object) {
            switch (object->GetType()) {
            case Object::ObjectType::PLAYER:
                Graphics::Get().DrawCircle(PLAYER_CIRCLE_RADIUS, object->GetCoordinates(), object->GetColor());
                break;

            case Object::ObjectType::BALL:
                // draw circle
                Graphics::Get().DrawCircle(BALL_CIRCLE_RADIUS, object->GetCoordinates(), object->GetColor());
                break;

            case Object::ObjectType::MONSTER:
                // draw triangle
                Graphics::Get().DrawCircle(MONSTER_CIRCLE_RADIUS, object->GetCoordinates(), object->GetColor());
                break;
            }
        }
    );

    Graphics::Get().EndFrame();
}

SceneGameOver::SceneGameOver()
{
    // TODO: non magic numbers
    m_GameOverTextLocation.x = 100.f;
    m_GameOverTextLocation.y = 100.f;
}

SceneGameOver::~SceneGameOver()
{
}

void SceneGameOver::Render()
{
    Graphics::Get().Clear();
    Graphics::Get().DrawText(m_GameOverTextLocation, L"GAME OVER", MEDIUM_FONT_SIZE);

    Client::Get().GetWorld().IterateAllObjects(
        [](const Object* object) {
            switch (object->GetType()) {
            case Object::ObjectType::PLAYER:
                Graphics::Get().DrawCircle(PLAYER_CIRCLE_RADIUS, object->GetCoordinates(), object->GetColor());
                break;

            case Object::ObjectType::BALL:
                // draw circle
                Graphics::Get().DrawCircle(BALL_CIRCLE_RADIUS, object->GetCoordinates(), object->GetColor());
                break;

            case Object::ObjectType::MONSTER:
                // draw triangle
                Graphics::Get().DrawCircle(MONSTER_CIRCLE_RADIUS, object->GetCoordinates(), object->GetColor());
                break;
            }
        }
    );

    Graphics::Get().EndFrame();
}
