/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Laser.h
Project:    CS230 Game
Author:     Jonathan Holmes, Sehoon Kim
Created:    May 31, 2025
*/

#pragma once
#include "Engine/GameObject.hpp"
#include "Engine/Matrix.hpp"
#include "GameObjectTypes.hpp"

class Laser : public CS230::GameObject
{
public:
    Laser(Math::vec2 pos, double rot, Math::vec2 scale_in, Math::vec2 vel);

    GameObjectTypes Type() override
    {
        return GameObjectTypes::Laser;
    }

    std::string TypeName() override
    {
        return "Laser";
    }

    void Update(double dt) override;
    bool CanCollideWith(GameObjectTypes other_object_type) override;
    void ResolveCollision(GameObject* other_object) override;

    static constexpr double DefaultVelocity = 600;
};
