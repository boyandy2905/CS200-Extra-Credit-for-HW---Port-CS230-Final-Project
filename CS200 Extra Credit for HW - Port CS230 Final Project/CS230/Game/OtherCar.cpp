/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  OtherCar.cpp
Project:    CS230 Game
Author:     Sehoon Kim, Chanwoong Moon
Created:    June 14, 2025
*/

#include "OtherCar.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Window.hpp"
#include "Mode3.hpp"

OtherCar::OtherCar(Math::vec2 pos) : GameObject(pos)
{
    int         sprite_index = 1 + (rand() % 5);
    std::string sprite_file  = "Assets/Enemy" + std::to_string(sprite_index) + ".spt";
    AddGOComponent(new CS230::Sprite(sprite_file, this));
    change_state(&state_driving);
}

void OtherCar::Update(double dt)
{
    GameObject::Update(dt);
    Math::vec2 camera_pos = Engine::GetGameStateManager().GetGSComponent<CS230::Camera>()->GetPosition();
    if (GetPosition().x + 80.0 < camera_pos.x)
    {
        Destroy();
    }
}

void OtherCar::Draw(Math::TransformationMatrix camera_matrix)
{
    CS230::GameObject::Draw(camera_matrix);
}

void OtherCar::ResolveCollision(GameObject* other_object)
{
    if (other_object->Type() == GameObjectTypes::Player)
    {
        if (static_cast<Player*>(other_object)->IsBoosting())
        {
            RemoveGOComponent<CS230::Collision>();
            change_state(&state_broken);
        }
    }
}

void OtherCar::Break()
{
    if (current_state == &state_broken)
    {
        return;
    }
    RemoveGOComponent<CS230::RectCollision>();
    SetVelocity({ 0, 0 });
    Destroy();
}

void OtherCar::State_Broken::Enter(GameObject* object)
{
    OtherCar* othercar = static_cast<OtherCar*>(object);

    othercar->RemoveGOComponent<CS230::Sprite>();
}

void OtherCar::State_Broken::Update(GameObject* object, [[maybe_unused]] double dt)
{
    OtherCar* othercar = static_cast<OtherCar*>(object);

    othercar->Destroy();
}

void OtherCar::State_Broken::CheckExit([[maybe_unused]] GameObject* object)
{
}

void OtherCar::State_Driving::Enter([[maybe_unused]] GameObject* object)
{
}

void OtherCar::State_Driving::Update([[maybe_unused]] GameObject* object, [[maybe_unused]] double dt)
{
    OtherCar* othercar = static_cast<OtherCar*>(object);

    othercar->UpdatePosition({ othercar->speed * dt, 0 });
}

void OtherCar::State_Driving::CheckExit([[maybe_unused]] GameObject* object)
{
}
