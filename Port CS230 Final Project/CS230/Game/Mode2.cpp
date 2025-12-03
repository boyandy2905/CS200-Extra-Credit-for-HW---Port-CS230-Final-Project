/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Mode2.cpp
Project:    CS230 Game
Author:     Jonathan Holmes, Sehoon Kim
Created:    March 13, 2025
*/

#include "Mode2.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Score.hpp"
#include "Engine/Sprite.hpp"
#include "Engine/Window.hpp"
#include "Engine/Input.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Font.hpp"
#include "Meteor.hpp"
#include "Ship.hpp"
#include "Fonts.hpp"
#include "States.hpp"
 
Mode2::Mode2()
{
    ship = nullptr;
}

void Mode2::Load()
{
#ifdef _DEBUG
    AddGSComponent(new CS230::ShowCollision());
#endif
    AddGSComponent(new CS230::GameObjectManager());
    AddGSComponent(new CS230::Score());

    ship = new Ship({ static_cast<double>(Engine::GetWindow().GetSize().x) / 2.0, static_cast<double>(Engine::GetWindow().GetSize().y) / 2.0 }, 0.0, { 0.75, 0.75 });
    GetGSComponent<CS230::GameObjectManager>()->Add(ship);

    AddGSComponent(new CS230::ParticleManager<Particles::Hit>());
    AddGSComponent(new CS230::ParticleManager<Particles::MeteorBit>());

    restart  = Engine::GetFont(static_cast<int>(Fonts::Outlined)).PrintToTexture("Press R To Restart", color);
    gameover = Engine::GetFont(static_cast<int>(Fonts::Outlined)).PrintToTexture("Game Over!", color);

    if (auto sc = GetGSComponent<CS230::Score>(); sc)
    {
        score = sc->Value();
    }
    else
    {
        score = 0;
    }
    update_score_text(score);
}

void Mode2::Update(double dt)
{
    UpdateGSComponents(dt);
    GetGSComponent<CS230::GameObjectManager>()->UpdateAll(dt);
    // Run collision tests so objects can resolve collisions (state/animation changes)
    GetGSComponent<CS230::GameObjectManager>()->CollisionTest();
    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Escape))
    {
        Engine::GetGameStateManager().RequestNextGameState(static_cast<int>(States::MainMenu));
        return; // avoid using members after unload
    }
    if (ship != nullptr && ship->Exploded() == true)
    {
        if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::R))
        {
            Engine::GetGameStateManager().ReloadState();
        }
    }
    if (ship != nullptr && ship->Exploded() == false)
    {
        last_time += dt;
        if (last_time >= meteor_timer)
        {
            GetGSComponent<CS230::GameObjectManager>()->Add(new Meteor());
            last_time = 0.0;
        }
        if (auto sc = GetGSComponent<CS230::Score>(); sc && score != sc->Value())
        {
            score = sc->Value();
            update_score_text(score);
        }
    }
}

void Mode2::Unload()
{
    ClearGSComponents();
}

void Mode2::Draw()
{
    Engine::GetWindow().Clear(0x000000FF);
    GetGSComponent<CS230::GameObjectManager>()->DrawAll(Math::TransformationMatrix());

    if (ship != nullptr && ship->Exploded() == true)
    {
        restart->Draw(Math::TranslationMatrix(Math::ivec2(Engine::GetWindow().GetSize().x / 2 - 240, Engine::GetWindow().GetSize().y - 240)));
        gameover->Draw(Math::TranslationMatrix(Math::ivec2(Engine::GetWindow().GetSize().x / 2 - 135, Engine::GetWindow().GetSize().y - 500)));
    }

    Math::ivec2 window_size = Engine::GetWindow().GetSize();
    score_texture->Draw(Math::TranslationMatrix(Math::ivec2(0, window_size.y - score_texture->GetSize().y)));
}

void Mode2::update_score_text(int value)
{
    score_texture = Engine::GetFont(static_cast<int>(Fonts::Simple)).PrintToTexture("Score: " + std::to_string(value), 0xFFFFFFFF);
}
