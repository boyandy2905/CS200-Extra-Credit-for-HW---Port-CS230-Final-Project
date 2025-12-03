/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Asteroid.h
Project:    CS230 Game
Author:     Jonathan Holmes, Sehoon Kim
Created:    March 26, 2025
*/

#pragma once
#include "Engine/GameObject.hpp"
#include "Engine/Matrix.hpp"
#include "GameObjectTypes.hpp"

class Asteroid : public CS230::GameObject
{
public:
    Asteroid(Math::vec2 start_position);

    GameObjectTypes Type() override
    {
        return GameObjectTypes::Asteroid;
    }

    std::string TypeName() override
    {
        return "Asteroid";
    }

    bool CanCollideWith(GameObjectTypes type) override;
    void ResolveCollision(GameObject* other_object) override;

private:
    static constexpr double bounce_velocity = 500;

    enum class Animations
    {
        None,
        Land
    };

    class State_Bouncing : public CS230::GameObject::State
    {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;

        std::string GetName() override
        {
            return "Bounce";
        }
    };

    State_Bouncing state_bouncing;

    class State_Landing : public CS230::GameObject::State
    {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;

        std::string GetName() override
        {
            return "Land";
        }
    };

    State_Landing state_landing;
};
