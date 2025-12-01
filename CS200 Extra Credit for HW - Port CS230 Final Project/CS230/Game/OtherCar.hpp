/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  OtherCar.h
Project:    CS230 Game
Author:     Sehoon Kim, Chanwoong Moon
Created:    June 14, 2025
*/

#pragma once
#include "Engine/GameObject.hpp"
#include "Engine/Timer.hpp"
#include "GameObjectTypes.hpp"
#include "Player.hpp"

class OtherCar : public CS230::GameObject
{
public:
    OtherCar(Math::vec2 position);
    void Update(double dt) override;
    void Draw(Math::TransformationMatrix camera_matrix) override;
    void ResolveCollision(GameObject* other_object) override;

    GameObjectTypes Type() override
    {
        return GameObjectTypes::OtherCar;
    }

    std::string TypeName() override
    {
        return "OtherCar";
    }

    void Break();

private:
    double speed = 50.0;

    class State_Broken : public CS230::GameObject::State
    {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;

        std::string GetName() override
        {
            return "Broken";
        }
    };

    State_Broken state_broken;

    class State_Driving : public CS230::GameObject::State
    {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;

        std::string GetName() override
        {
            return "Driving";
        }
    };

    State_Driving state_driving;
};
