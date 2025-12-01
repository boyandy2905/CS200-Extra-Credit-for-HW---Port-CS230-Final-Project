/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Mode1.cpp
Project:    CS230 Game
Author:     Jonathan Holmes, Sehoon Kim
Created:    March 13, 2025
*/

#include "Mode1.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Window.hpp"
#include "Engine/Input.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Score.hpp"
#include "Engine/Font.hpp"
#include "Asteroid.hpp"
#include "Cat.hpp"
#include "Crates.hpp"
#include "Robot.hpp"
#include "Floor.hpp"
#include "Fonts.hpp"
#include "Gravity.hpp"
#include "Portal.hpp"
#include "States.hpp"
#include <iostream>
 
Mode1::Mode1()
{
}

void Mode1::Load()
{
#ifdef _DEBUG
    AddGSComponent(new CS230::ShowCollision());
#endif

    AddGSComponent(new CS230::Camera(Math::rect{
        Math::vec2(0.15 * static_cast<double>(Engine::GetWindow().GetSize().x), 0.0),
        Math::vec2(0.35 * static_cast<double>(Engine::GetWindow().GetSize().x), 0.0)
    }));
    AddGSComponent(new Gravity(Mode1::gravity));
    AddGSComponent(new CS230::Timer(timer_max));
    AddGSComponent(new CS230::GameObjectManager());
    AddGSComponent(new Background());
    AddGSComponent(new CS230::Score());

    GetGSComponent<Background>()->Add("Assets/Planets.png", 0.25);
    GetGSComponent<Background>()->Add("Assets/Ships.png", 0.5);
    GetGSComponent<Background>()->Add("Assets/Foreground.png", 1.0);

    starting_floor_ptr = new Floor(
        Math::irect{
            {   0,                       0 },
            { 930, static_cast<int>(floor) }
    });
    GetGSComponent<CS230::GameObjectManager>()->Add(starting_floor_ptr);
    GetGSComponent<CS230::GameObjectManager>()->Add(new Floor(
        Math::irect{
            { 1014,                       0 },
            { 2700, static_cast<int>(floor) }
    }));
    GetGSComponent<CS230::GameObjectManager>()->Add(new Floor(
        Math::irect{
            { 2884,                       0 },
            { 4126, static_cast<int>(floor) }
    }));
    GetGSComponent<CS230::GameObjectManager>()->Add(new Floor(
        Math::irect{
            { 4208,                       0 },
            { 5760, static_cast<int>(floor) }
    }));

    cat_ptr = new Cat({ 300, floor }, starting_floor_ptr);
    GetGSComponent<CS230::GameObjectManager>()->Add(cat_ptr);

    GetGSComponent<CS230::GameObjectManager>()->Add(new Asteroid({ 600, floor }));
    GetGSComponent<CS230::GameObjectManager>()->Add(new Asteroid({ 1800, floor }));
    GetGSComponent<CS230::GameObjectManager>()->Add(new Asteroid({ 2400, floor }));

    GetGSComponent<CS230::GameObjectManager>()->Add(new Crates({ 900, floor }, 2));
    GetGSComponent<CS230::GameObjectManager>()->Add(new Crates({ 1400, floor }, 1));
    GetGSComponent<CS230::GameObjectManager>()->Add(new Crates({ 2000, floor }, 5));
    GetGSComponent<CS230::GameObjectManager>()->Add(new Crates({ 4000, floor }, 3));
    GetGSComponent<CS230::GameObjectManager>()->Add(new Crates({ 5450, floor }, 1));
    GetGSComponent<CS230::GameObjectManager>()->Add(new Crates({ 5525, floor }, 3));
    GetGSComponent<CS230::GameObjectManager>()->Add(new Crates({ 5600, floor }, 5));

    GetGSComponent<CS230::GameObjectManager>()->Add(new Robot({ 1025, floor }, cat_ptr, 1025, 1350));
    GetGSComponent<CS230::GameObjectManager>()->Add(new Robot({ 2050, floor }, cat_ptr, 2050, 2325));
    GetGSComponent<CS230::GameObjectManager>()->Add(new Robot({ 3400, floor }, cat_ptr, 3400, 3800));
    GetGSComponent<CS230::GameObjectManager>()->Add(new Robot({ 4225, floor }, cat_ptr, 4225, 4800));

    GetGSComponent<CS230::GameObjectManager>()->Add(new Portal(
        static_cast<int>(States::MainMenu), Math::irect{
                                                { 5750,   0 },
                                                { 5760, 600 }
    }));

    AddGSComponent(new CS230::ParticleManager<Particles::Smoke>());

    GetGSComponent<CS230::Camera>()->SetPosition({ 0, 0 });
    GetGSComponent<CS230::Camera>()->SetLimit(
        Math::irect{
            Math::ivec2(0, 0),
            GetGSComponent<Background>()->GetSize() - Engine::GetWindow().GetSize()
    });

    last_timer = static_cast<int>(timer_max);
    update_timer_text(last_timer);

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

void Mode1::Update(double dt)
{
    UpdateGSComponents(dt);
    GetGSComponent<CS230::GameObjectManager>()->UpdateAll(dt);
    // Run collision tests so objects can resolve collisions (state/animation changes)
    GetGSComponent<CS230::GameObjectManager>()->CollisionTest();
    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Escape))
    {
        Engine::GetGameStateManager().SetNextGameState(static_cast<int>(States::MainMenu));
        return; // avoid accessing members after unload
    }
    if (auto cam = GetGSComponent<CS230::Camera>(); cam)
    {
        cam->Update(cat_ptr->GetPosition());
    }

    if (auto t = GetGSComponent<CS230::Timer>(); t && t->Remaining() < static_cast<double>(last_timer))
    {
        last_timer = static_cast<int>(t->Remaining());
        update_timer_text(last_timer);
    }
    if (last_timer == 0)
    {
        Engine::GetGameStateManager().SetNextGameState(static_cast<int>(States::MainMenu));
        return; // avoid accessing members after unload
    }

    if (auto sc = GetGSComponent<CS230::Score>(); sc && score != sc->Value())
    {
        score = sc->Value();
        update_score_text(score);
    }
}

void Mode1::Unload()
{
    cat_ptr = nullptr;
    ClearGSComponents();
}

void Mode1::Draw()
{
    Engine::GetWindow().Clear(0x000000FF);
    if (auto cam = GetGSComponent<CS230::Camera>(); cam)
    {
        Math::TransformationMatrix camera_matrix = cam->GetMatrix();
        GetGSComponent<Background>()->Draw(*cam);
        GetGSComponent<CS230::GameObjectManager>()->DrawAll(camera_matrix);
    }
    else
    {
        Engine::GetLogger().LogDebug("Mode1::Draw: camera missing, drawing with identity transform");
        GetGSComponent<CS230::GameObjectManager>()->DrawAll(Math::TransformationMatrix());
    }

    Math::ivec2 window_size = Engine::GetWindow().GetSize();
    timer_texture->Draw(Math::TranslationMatrix(Math::ivec2(window_size.x - 10 - timer_texture->GetSize().x, window_size.y - timer_texture->GetSize().y - 5)));
    score_texture->Draw(Math::TranslationMatrix(Math::ivec2(window_size.x - 10 - score_texture->GetSize().x, window_size.y - score_texture->GetSize().y - 100)));
}

void Mode1::update_timer_text(int value)
{
    timer_texture = Engine::GetFont(static_cast<int>(Fonts::Simple)).PrintToTexture("Timer: " + std::to_string(value), 0xFFFFFFFF);
}

void Mode1::update_score_text(int value)
{
    score_texture = Engine::GetFont(static_cast<int>(Fonts::Simple)).PrintToTexture("Score: " + std::to_string(value), 0xFFFFFFFF);
}
