/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Floor.h
Project:    CS230 Game
Author:     Jonathan Holmes, Sehoon Kim
Created:    May 27, 2025
*/

#pragma once
#include "Engine/GameObject.hpp"
#include "Engine/Matrix.hpp"
#include "GameObjectTypes.hpp"

class Floor : public CS230::GameObject
{
public:
    Floor(Math::irect boundary);

    virtual GameObjectTypes Type() override
    {
        return GameObjectTypes::Floor;
    }

    std::string TypeName() override
    {
        return "Floor";
    }
};
