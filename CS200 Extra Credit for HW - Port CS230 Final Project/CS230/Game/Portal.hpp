/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Portal.h
Project:    CS230 Game
Author:     Jonathan Holmes, Sehoon Kim
Created:    May 27, 2025
*/

#pragma once
#include "Engine/GameObject.hpp"
#include "Engine/Matrix.hpp"
#include "GameObjectTypes.hpp"

class Portal : public CS230::GameObject
{
public:
    Portal(int to_state, Math::irect boundary);

    virtual GameObjectTypes Type() override
    {
        return GameObjectTypes::Portal;
    }

    std::string TypeName() override
    {
        return "Portal";
    }

    void GoToState();

private:
    int to_state;
};
