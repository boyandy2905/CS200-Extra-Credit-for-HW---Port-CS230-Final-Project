/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Ship.cpp
Project:    CS230 Game
Author:     Jonathan Holmes, Sehoon Kim
Created:    March 22, 2025
*/

#include "Ship.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Window.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Logger.hpp"
#include "Engine/Input.hpp"
#include "Laser.hpp"
#include "ScreenWrap.hpp"

Ship::Ship(Math::vec2 pos, double rot, Math::vec2 ship_scale) : GameObject(pos, rot, ship_scale)
{
    AddGOComponent(new CS230::Sprite("Assets/Ship.spt", this));
    flame_left.Load("Assets/Flame.spt", this);
    flame_right.Load("Assets/Flame.spt", this);
    flame_left.PlayAnimation(static_cast<int>(Flame_Animations::None));
    flame_right.PlayAnimation(static_cast<int>(Flame_Animations::None));
    AddGOComponent(new ScreenWrap(*this));

    exploded = false;
}

void Ship::Update(double dt)
{
    flame_left.Update(dt);
    flame_right.Update(dt);
    GetMatrix();
    if (exploded == false)
    {
        if (Engine::GetInput().KeyDown(CS230::Input::Keys::W))
        {
            if (leftflame == false && rightflame == false)
            {
                flame_left.PlayAnimation(static_cast<int>(Flame_Animations::Flame));
                flame_right.PlayAnimation(static_cast<int>(Flame_Animations::Flame));
                leftflame  = true;
                rightflame = true;
            }
            UpdateVelocity(Math::RotationMatrix(GetRotation()) * Math::vec2(0, speed * dt));
        }
        if (Engine::GetInput().KeyDown(CS230::Input::Keys::W) == false)
        {
            flame_left.PlayAnimation(static_cast<int>(Flame_Animations::None));
            flame_right.PlayAnimation(static_cast<int>(Flame_Animations::None));
            leftflame  = false;
            rightflame = false;
        }
        if (Engine::GetInput().KeyDown(CS230::Input::Keys::A))
        {
            SetRotation(GetRotation() + rotation_speed * dt);
        }
        if (Engine::GetInput().KeyDown(CS230::Input::Keys::D))
        {
            SetRotation(GetRotation() - rotation_speed * dt);
        }

        if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Enter))
        {
            Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>()->Add(new Laser(
                GetMatrix() * Math::vec2(GetGOComponent<CS230::Sprite>()->GetHotSpot(3)), GetRotation(), GetScale(), Math::RotationMatrix(GetRotation()) * Math::vec2(0, Laser::DefaultVelocity)));
            Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>()->Add(new Laser(
                GetMatrix() * Math::vec2(GetGOComponent<CS230::Sprite>()->GetHotSpot(4)), GetRotation(), GetScale(), Math::RotationMatrix(GetRotation()) * Math::vec2(0, Laser::DefaultVelocity)));
        }
    }
    SetVelocity(Math::vec2(GetVelocity().x - GetVelocity().x * drag * dt, GetVelocity().y - GetVelocity().y * drag * dt));
    // Engine::GetLogger().LogDebug("Velocity: " + std::to_string(GetVelocity().x) + ", " + std::to_string(GetVelocity().y));
    UpdatePosition(Math::vec2(GetVelocity().x * dt, GetVelocity().y * dt));

    UpdateGOComponents(dt);
}

void Ship::Draw(Math::TransformationMatrix camera_matrix)
{
    GetGOComponent<CS230::Sprite>()->Draw(camera_matrix * GetMatrix());
    if (exploded == false)
    {
        flame_left.Draw(camera_matrix * GetMatrix() * Math::TranslationMatrix(GetGOComponent<CS230::Sprite>()->GetHotSpot(1)));
        flame_right.Draw(camera_matrix * GetMatrix() * Math::TranslationMatrix(GetGOComponent<CS230::Sprite>()->GetHotSpot(2)));
    }
    // Also draw collision boxes for the ship if enabled (per-state or global)
    CS230::ShowCollision* show = Engine::GetGameStateManager().GetGSComponent<CS230::ShowCollision>();
    if (show == nullptr)
    {
        show = Engine::GetGlobalShowCollision();
    }
    if (show != nullptr && show->Enabled())
    {
        CS230::Collision* collision = GetGOComponent<CS230::Collision>();
        if (collision != nullptr)
        {
            collision->Draw(camera_matrix);
        }
    }
}

bool Ship::CanCollideWith(GameObjectTypes)
{
    return true;
}

void Ship::ResolveCollision(GameObject* other_object)
{
    double ship_radius  = GetGOComponent<CS230::CircleCollision>()->GetRadius();
    double other_radius = other_object->GetGOComponent<CS230::CircleCollision>()->GetRadius();
    double sum_radius   = ship_radius + other_radius;

    Math::vec2 ship_position  = GetPosition();
    Math::vec2 other_position = other_object->GetPosition();
    double     diff_x         = ship_position.x - other_position.x;
    double     diff_y         = ship_position.y - other_position.y;
    double     distance       = sqrt(diff_x * diff_x + diff_y * diff_y);

    if (distance < sum_radius)
    {
        GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Explosion));
        RemoveGOComponent<CS230::Collision>();
        exploded = true;
    }
}

bool Ship::Exploded()
{
    return exploded;
}
