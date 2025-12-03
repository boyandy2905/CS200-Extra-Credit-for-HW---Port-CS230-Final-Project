/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Ship.h
Project:    CS230 Game
Author:     Jonathan Holmes, Sehoon Kim
Created:    March 22, 2025
*/

#pragma once
#include "Engine/Collision.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/GameObjectManager.hpp"
#include "Engine/Input.hpp"
#include "Engine/Matrix.hpp"
#include "GameObjectTypes.hpp"

class Ship : public CS230::GameObject
{
public:
    Ship(Math::vec2 position, double rotation, Math::vec2 ship_scale);
    virtual void Update(double dt) override;
    virtual void Draw(Math::TransformationMatrix camera_matrix) override;

    GameObjectTypes Type() override
    {
        return GameObjectTypes::Ship;
    }

    std::string TypeName() override
    {
        return "Ship";
    }

    bool CanCollideWith(GameObjectTypes) override;
    void ResolveCollision(GameObject* other_object) override;
    bool Exploded();

    enum class Animations
    {
        None,
        Explosion
    };

    enum class Flame_Animations
    {
        None,
        Flame
    };

private:
    static constexpr double speed          = 700;
    static constexpr double drag           = 1;
    static constexpr double rotation_speed = 5;
    CS230::Sprite           flame_left;
    CS230::Sprite           flame_right;
    bool                    leftflame;
    bool                    rightflame;

    bool exploded = false;
};
