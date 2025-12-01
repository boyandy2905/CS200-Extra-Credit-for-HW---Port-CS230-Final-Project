/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Player.cpp
Project:    CS230 Game
Author:     Sehoon Kim, Chanwoong Moon
Created:    June 14, 2025
*/

#include "Player.hpp"
#include "Engine/Engine.hpp"
#include "OtherCar.hpp"
#include "Mode3.hpp"
#include "States.hpp"
#include "Engine/Window.hpp"
#include "Engine/Input.hpp"
#include <cmath>

Player::Player(Math::vec2 start_position) : GameObject(start_position)
{
    AddGOComponent(new CS230::Sprite("Assets/Car.spt", this));

    Boost_On.Load("Assets/Boost.spt", this);
    Boost_On.PlayAnimation(static_cast<int>(Boost_Animations::None));

    hurt_timer           = new CS230::Timer(0.0);
    boost_timer          = new CS230::Timer(0.0);
    boost_cooldown_timer = new CS230::Timer(0.0);
    AddGOComponent(hurt_timer);
    AddGOComponent(boost_timer);
    AddGOComponent(boost_cooldown_timer);


    SetVelocity({ 0, 0 });
}

void Player::Update(double dt)
{
    GameObject::Update(dt);
    Boost_On.Update(dt);
    CS230::RectCollision* collider = GetGOComponent<CS230::RectCollision>();
    if (auto pm = Engine::GetGameStateManager().GetGSComponent<CS230::ParticleManager<Particles::Exhaust>>(); pm)
    {
        pm->Emit(1, GetPosition(), Math::vec2(0, 0), Math::vec2(-100, 0), 2.0 * std::acos(-1.0) / 3.0);
    }
    if (collider->WorldBoundary().Left() < Engine::GetGameStateManager().GetGSComponent<CS230::Camera>()->GetPosition().x)
    {
        UpdatePosition(Math::vec2(Engine::GetGameStateManager().GetGSComponent<CS230::Camera>()->GetPosition().x - collider->WorldBoundary().Left(), 0));
        SetVelocity(Math::vec2(0, GetVelocity().y));
    }
    if (collider->WorldBoundary().Right() > Engine::GetGameStateManager().GetGSComponent<CS230::Camera>()->GetPosition().x + Engine::GetWindow().GetSize().x)
    {
        UpdatePosition(Math::vec2(Engine::GetGameStateManager().GetGSComponent<CS230::Camera>()->GetPosition().x + Engine::GetWindow().GetSize().x - collider->WorldBoundary().Right(), 0));
        SetVelocity(Math::vec2(0, GetVelocity().y));
    }
    if (collider->WorldBoundary().Bottom() - 20 < Engine::GetGameStateManager().GetGSComponent<CS230::Camera>()->GetPosition().y)
    {
        UpdatePosition(Math::vec2(0, Engine::GetGameStateManager().GetGSComponent<CS230::Camera>()->GetPosition().y - collider->WorldBoundary().Bottom() + 20));
        SetVelocity(Math::vec2(GetVelocity().x, GetVelocity().y));
    }
    if (collider->WorldBoundary().Top() + 20 > Engine::GetGameStateManager().GetGSComponent<CS230::Camera>()->GetPosition().y + Engine::GetWindow().GetSize().y)
    {
        UpdatePosition(Math::vec2(0, Engine::GetGameStateManager().GetGSComponent<CS230::Camera>()->GetPosition().y + Engine::GetWindow().GetSize().y - collider->WorldBoundary().Top() - 20));
        SetVelocity(Math::vec2(GetVelocity().x, GetVelocity().y));
    }

    double current_acceleration = x_acceleration;
    double current_max_velocity = max_velocity;
    if (is_boosting)
    {
        current_acceleration *= 2;
        current_max_velocity *= 2;
    }

    if (Engine::GetInput().KeyDown(CS230::Input::Keys::W))
    {
        SetVelocity(Math::vec2(GetVelocity().x + current_acceleration * dt, GetVelocity().y));
        if (GetVelocity().x > current_max_velocity)
        {
            SetVelocity(Math::vec2(current_max_velocity, GetVelocity().y));
        }
    }
    if (Engine::GetInput().KeyDown(CS230::Input::Keys::S))
    {
        SetVelocity(Math::vec2(GetVelocity().x - current_acceleration * dt, GetVelocity().y));
        if (GetVelocity().x < min_velocity)
        {
            SetVelocity(Math::vec2(min_velocity, GetVelocity().y));
        }
    }
    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::A))
    {
        SetPosition(Math::vec2(GetPosition().x, GetPosition().y + 125));
    }
    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::D))
    {
        SetPosition(Math::vec2(GetPosition().x, GetPosition().y - 125));
    }

    if ((Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Space) || Engine::GetInput().KeyJustPressed(CS230::Input::Keys::LeftShift)) && boost_cooldown_timer->Remaining() <= 0.0)
    {
        boost_timer->Set(boost_time);
        is_boosting  = true;
        was_boosting = true;
        Boost_On.PlayAnimation(static_cast<int>(Boost_Animations::Boost));
    }
    if (is_boosting && boost_timer->Remaining() <= 0.0)
    {
        is_boosting = false;
        boost_cooldown_timer->Set(5.0);
        Boost_On.PlayAnimation(static_cast<int>(Boost_Animations::None));
    }
    if (!is_boosting && GetVelocity().x > max_velocity)
    {
        double dec_velocity = GetVelocity().x - x_acceleration * dt;
        if (dec_velocity < max_velocity)
        {
            dec_velocity = max_velocity;
        }
        SetVelocity({ dec_velocity, GetVelocity().y });
    }
}

bool Player::CanCollideWith(GameObjectTypes other_object_type)
{
    return other_object_type != GameObjectTypes::Particle;
}

void Player::ResolveCollision(GameObject* other_object)
{
    if (other_object->Type() == GameObjectTypes::OtherCar)
    {
        if (is_boosting == true)
        {
            static_cast<OtherCar*>(other_object)->Break();
        }
        else if (hurt_timer->Remaining() <= 0.0)
        {
            hurt_timer->SetBlinkPeriod(0.1);
            hurt_timer->Set(hurt_time);
            SetVelocity(Math::vec2(GetVelocity().x * 0.3, GetVelocity().y));
            if (auto pm = Engine::GetGameStateManager().GetGSComponent<CS230::ParticleManager<Particles::Bump>>(); pm)
            {
                pm->Emit(2, GetPosition(), Math::vec2(0, 0), Math::vec2(0, 100), 2.0 * std::acos(-1.0) / 3.0);
            }
        }
        return;
    }

    if (other_object->Type() == GameObjectTypes::Portal)
    {
        static_cast<Portal*>(other_object)->GoToState();
    }
}

void Player::Draw(Math::TransformationMatrix camera_matrix)
{
    if (hurt_timer->Remaining() == 0.0 || hurt_timer->TickTock() == true)
    {
        CS230::GameObject::Draw(camera_matrix);
    }
    if (is_boosting)
    {
        Boost_On.Draw(camera_matrix * GetMatrix() * Math::TranslationMatrix(GetGOComponent<CS230::Sprite>()->GetHotSpot(1)));
    }
}
