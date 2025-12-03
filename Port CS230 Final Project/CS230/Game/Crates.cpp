/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Crates.cpp
Project:    CS230 Game
Author:     Jonathan Holmes, Sehoon Kim
Created:    April 27, 2025
*/

#include "Crates.hpp"
#include "Engine/Engine.hpp"
#include "Mode1.hpp"

Crates::Crates(Math::vec2 pos, int size) : CS230::GameObject(pos)
{
    switch (size)
    {
        case 1:
        AddGOComponent(new CS230::Sprite("Assets/Crates1.spt", this));
        break;

        case 2:
        AddGOComponent(new CS230::Sprite("Assets/Crates2.spt", this));
        break;

        case 3:
        AddGOComponent(new CS230::Sprite("Assets/Crates3.spt", this));
        break;

        case 5:
        AddGOComponent(new CS230::Sprite("Assets/Crates5.spt", this));
        break;
    }
}
