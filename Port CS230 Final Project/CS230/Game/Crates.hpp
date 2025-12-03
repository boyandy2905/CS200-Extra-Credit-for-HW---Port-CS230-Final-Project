/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Crates.h
Project:    CS230 Game
Author:     Jonathan Holmes, Sehoon Kim
Created:    April 27, 2025
*/

#pragma once
#include "Engine/GameObject.hpp"
#include "Engine/Matrix.hpp"
#include "GameObjectTypes.hpp"

class Crates : public CS230::GameObject
{
public:
    Crates(Math::vec2 pos, int size);

    GameObjectTypes Type() override
    {
        return GameObjectTypes::Crates;
    }

    std::string TypeName() override
    {
        return "Crates";
    }
};
