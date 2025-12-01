/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Mode3.cpp
Project:    CS230 Game
Author:     Jonathan Holmes, Sehoon Kim, Chanwoong Moon
Created:    June 14, 2025
*/

#include "Mode3.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Window.hpp"
#include "Engine/Input.hpp"
#include "Engine/Particle.hpp"
#include "OtherCar.hpp"
#include "OtherCarManager.hpp"
#include "Player.hpp"
#include "Fonts.hpp"
#include "Portal.hpp"
#include "States.hpp"
#include <iostream>
#include "Engine/Font.hpp"
 
Mode3::Mode3() : player_ptr(nullptr), last_time(0.0), timer_texture(nullptr), last_timer(static_cast<int>(timer_max)), distance_texture(nullptr), total_distance(0.0)
{
}

void Mode3::Load()
{
#ifdef _DEBUG
    AddGSComponent(new CS230::ShowCollision());
#endif

    AddGSComponent(new CS230::Camera(Math::rect{
        Math::vec2(0.15 * static_cast<double>(Engine::GetWindow().GetSize().x), 0.0),
        Math::vec2(0.35 * static_cast<double>(Engine::GetWindow().GetSize().x), 0.0)
    }));
    AddGSComponent(new CS230::Timer(timer_max));
    AddGSComponent(new CS230::GameObjectManager());
    AddGSComponent(new Background());

    GetGSComponent<Background>()->Add("Assets/Road.png", 1.0);
    AddGSComponent(new CS230::ParticleManager<Particles::Exhaust>());
    player_ptr = new Player(Math::vec2(0.0, static_cast<double>(Engine::GetWindow().GetSize().y) / 2.0));
    GetGSComponent<CS230::GameObjectManager>()->Add(player_ptr);


    GetGSComponent<CS230::GameObjectManager>()->Add(new Portal(
        static_cast<int>(States::MainMenu), Math::irect{
                                                { 14390,   0 },
                                                { 14400, 600 }
    }));

    GetGSComponent<CS230::Camera>()->SetPosition({ 0, 0 });
    GetGSComponent<CS230::Camera>()->SetLimit(Math::irect{
            Math::ivec2(0,0),
            GetGSComponent<Background>()->GetSize() - Engine::GetWindow().GetSize()
    });

    GetGSComponent<CS230::GameObjectManager>()->Add(new OtherCarManager(this));

    AddGSComponent(new CS230::ParticleManager<Particles::Bump>());

    last_timer = static_cast<int>(timer_max);
    update_timer_text(last_timer);

    update_distance_text(static_cast<int>(total_distance));
}

void Mode3::Update(double dt)
{
    UpdateGSComponents(dt);
    GetGSComponent<CS230::GameObjectManager>()->UpdateAll(dt);
    // Run collision tests so objects can resolve collisions (state/animation changes)
    GetGSComponent<CS230::GameObjectManager>()->CollisionTest();

    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Escape))
    {
        Engine::GetGameStateManager().SetNextGameState(static_cast<int>(States::MainMenu));
        return; // state popped; avoid using this object's members after unload
    }
    if (auto cam = GetGSComponent<CS230::Camera>(); cam)
    {
        cam->Update(player_ptr->GetPosition());
    }

    if (auto t = GetGSComponent<CS230::Timer>(); t && t->Remaining() < static_cast<double>(last_timer))
    {
        last_timer = static_cast<int>(t->Remaining());
        update_timer_text(last_timer);
    }
    if (last_timer == 0)
    {
        Engine::GetGameStateManager().SetNextGameState(static_cast<int>(States::MainMenu));
        return; // state popped; avoid using this object's members after unload
    }

    total_distance += player_ptr->GetVelocity().x * dt;
    int cur_dist = static_cast<int>(total_distance);
    if (cur_dist != last_distance)
    {
        last_distance = cur_dist;
        update_distance_text(cur_dist);
    }
}

void Mode3::Unload()
{
    timer_texture.reset();
    distance_texture.reset();

    player_ptr     = nullptr;
    total_distance = 0;
    ClearGSComponents();
}

void Mode3::Draw()
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
        // Camera missing: skip camera-dependent background draw and draw objects without transform
        auto bg = GetGSComponent<Background>();
        if (bg)
        {
            // unable to draw background without camera
            Engine::GetLogger().LogDebug("Mode3::Draw: camera missing, skipping background draw");
        }
        GetGSComponent<CS230::GameObjectManager>()->DrawAll(Math::TransformationMatrix());
    }

    Math::ivec2 window_size = Engine::GetWindow().GetSize();
    timer_texture->Draw(Math::TranslationMatrix(Math::ivec2(window_size.x - 10 - timer_texture->GetSize().x, window_size.y - timer_texture->GetSize().y - 5)));
    distance_texture->Draw(Math::TranslationMatrix(Math::ivec2(window_size.x - 10 - distance_texture->GetSize().x, window_size.y - distance_texture->GetSize().y - 100)));
}

void Mode3::update_timer_text(int value)
{
    timer_texture = Engine::GetFont(static_cast<int>(Fonts::Simple)).PrintToTexture("Timer: " + std::to_string(value), 0xFFFFFFFF);
}

void Mode3::update_distance_text(int distance)
{
    distance_texture = Engine::GetFont(static_cast<int>(Fonts::Simple)).PrintToTexture("Distance: " + std::to_string(distance), 0xFFFFFFFF);
}
