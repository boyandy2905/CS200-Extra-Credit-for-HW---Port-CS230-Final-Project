/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Particle.cpp
Project:    CS230 Engine
Author:     Jonathan Holmes, Sehoon Kim
Created:    June 7, 2025
*/

#include "Particle.hpp"
#include "Engine.hpp"
#include "Sprite.hpp"

CS230::Particle::Particle(const std::filesystem::path& sprite_file) : GameObject(Math::vec2{ 0, 0 })
{
    life = 0.0;
    AddGOComponent(new CS230::Sprite(sprite_file, this));
}

void CS230::Particle::Start(Math::vec2 new_position, Math::vec2 new_velocity, double max_life)
{
    this->SetPosition(new_position);
    this->SetVelocity(new_velocity);
    life = max_life;

    GetGOComponent<CS230::Sprite>()->PlayAnimation(0);
}

void CS230::Particle::Update(double dt)
{
    if (Alive())
    {
        life -= dt;
        GameObject::Update(dt);
    }
}

void CS230::Particle::Draw(Math::TransformationMatrix camera_matrix)
{
    if (Alive())
    {
        GameObject::Draw(camera_matrix);
    }
}
