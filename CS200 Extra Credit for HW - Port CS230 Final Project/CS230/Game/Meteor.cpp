/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Meteor.cpp
Project:    CS230 Game
Author:     Jonathan Holmes, Sehoon Kim
Created:    April 27, 2025
*/

#include "Meteor.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Score.hpp"
#include "Engine/Window.hpp"
#include "Mode2.hpp"
#include <cmath>

// define PI if not provided
#ifndef PI
# ifdef M_PI
#  define PI M_PI
# else
static constexpr double PI = 3.14159265358979323846;
# endif
#endif

Meteor::Meteor() : Meteor(nullptr)
{
}

Meteor::Meteor(Meteor* parent) : CS230::GameObject({ 0, 0 })
{
    AddGOComponent(new CS230::Sprite("Assets/Meteor.spt", this));
    if (parent == nullptr)
    {
        SetVelocity(Math::vec2{ (static_cast<double>(rand()) / RAND_MAX) * default_velocity * 2 - default_velocity, (static_cast<double>(rand()) / RAND_MAX) * default_velocity * 2 - default_velocity });

        double check_meteor_x  = static_cast<double>(rand()) / RAND_MAX;
        double random_meteor_x = (static_cast<double>(rand()) / RAND_MAX) * Engine::GetWindow().GetSize().x;
        double meteor_x = (check_meteor_x >= 0.5) ? Engine::GetWindow().GetSize().x + GetGOComponent<CS230::CircleCollision>()->GetRadius() : -GetGOComponent<CS230::CircleCollision>()->GetRadius();

        double check_meteor_y  = static_cast<double>(rand()) / RAND_MAX;
        double random_meteor_y = (static_cast<double>(rand()) / RAND_MAX) * Engine::GetWindow().GetSize().y;
        double meteor_y = (check_meteor_y >= 0.5) ? Engine::GetWindow().GetSize().y + GetGOComponent<CS230::CircleCollision>()->GetRadius() : -GetGOComponent<CS230::CircleCollision>()->GetRadius();

        int pos_case = rand() % 3;
        switch (pos_case)
        {
            case 0: SetPosition({ meteor_x, random_meteor_y }); break;
            case 1: SetPosition({ random_meteor_x, meteor_y }); break;
            case 2: SetPosition({ meteor_x, meteor_y }); break;
        }

        SetRotation((static_cast<double>(rand()) / RAND_MAX) * 2 * PI);

        size = static_cast<int>(default_size);
    }
    else
    {
        SetVelocity(parent->GetVelocity());
        SetPosition(parent->GetPosition());
        SetRotation(parent->GetRotation());

        size = parent->size - 1;
    }
    health = default_healths[size];
    SetScale(Math::vec2{ default_scales[size], default_scales[size] });
}

void Meteor::Update(double dt)
{
    GameObject::Update(dt);

    GetPosition();
    GetGOComponent<CS230::Sprite>()->GetFrameSize();
    Engine::GetWindow().GetSize();

    if (GetPosition().x + GetGOComponent<CS230::Sprite>()->GetFrameSize().x / 2.0 < 0)
    {
        SetPosition({ GetPosition().x + Engine::GetWindow().GetSize().x + GetGOComponent<CS230::Sprite>()->GetFrameSize().x, GetPosition().y });
    }
    if (GetPosition().x - GetGOComponent<CS230::Sprite>()->GetFrameSize().x / 2.0 > Engine::GetWindow().GetSize().x)
    {
        SetPosition({ GetPosition().x - Engine::GetWindow().GetSize().x - GetGOComponent<CS230::Sprite>()->GetFrameSize().x, GetPosition().y });
    }
    if (GetPosition().y + GetGOComponent<CS230::Sprite>()->GetFrameSize().y / 2.0 < 0)
    {
        SetPosition({ GetPosition().x, GetPosition().y + Engine::GetWindow().GetSize().y + GetGOComponent<CS230::Sprite>()->GetFrameSize().y });
    }
    if (GetPosition().y - GetGOComponent<CS230::Sprite>()->GetFrameSize().y / 2.0 > Engine::GetWindow().GetSize().y)
    {
        SetPosition({ GetPosition().x, GetPosition().y - Engine::GetWindow().GetSize().y - GetGOComponent<CS230::Sprite>()->GetFrameSize().y });
    }

    if (GetGOComponent<CS230::Sprite>() != nullptr)
    {
        if (GetGOComponent<CS230::Sprite>()->CurrentAnimation() == static_cast<int>(Animations::Fade))
        {
            if (GetGOComponent<CS230::Sprite>()->AnimationEnded())
            {
                Destroy();
            }
        }
    }
}

void Meteor::ResolveCollision(GameObject* other_object)
{
    UpdateVelocity(0.01 * other_object->GetVelocity());

    Math::vec2 meteor_pos = GetPosition();
    Math::vec2 laser_pos  = other_object->GetPosition();
    Math::vec2 direction  = (laser_pos - meteor_pos).Normalize();

    Math::vec2 meteor_surface = meteor_pos + direction * (GetGOComponent<CS230::CircleCollision>()->GetRadius());

    if (auto pm_hit = Engine::GetGameStateManager().GetGSComponent<CS230::ParticleManager<Particles::Hit>>(); pm_hit)
    {
        pm_hit->Emit(1, meteor_surface, { 0, 0 }, direction, 0.0);
    }
    if (auto pm_meteor = Engine::GetGameStateManager().GetGSComponent<CS230::ParticleManager<Particles::MeteorBit>>(); pm_meteor)
    {
        pm_meteor->Emit(5, meteor_surface, -other_object->GetVelocity() / 5, direction * 100, PI / 3);
    }


    health -= 1;
    if (health < 0)
    {
        RemoveGOComponent<CS230::Collision>();
        GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Fade));
        if (auto score_comp = Engine::GetGameStateManager().GetGSComponent<CS230::Score>(); score_comp)
        {
            score_comp->Add(10);
        }
        if (size > 0)
        {
            Meteor* meteor_1 = new Meteor(this);
            meteor_1->SetVelocity(Math::RotationMatrix(PI / 6) * GetVelocity());

            Meteor* meteor_2 = new Meteor(this);
            meteor_2->SetVelocity(Math::RotationMatrix(-PI / 6) * GetVelocity());

            Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>()->Add(meteor_1);
            Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>()->Add(meteor_2);
        }
    }
}
