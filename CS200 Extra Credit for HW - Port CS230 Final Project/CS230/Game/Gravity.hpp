/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Gravity.h
Project:    CS230 Game
Author:     Jonathan Holmes, Sehoon Kim
Created:    May 12, 2025
*/

#pragma once
#include "Engine/Component.hpp"

class Gravity : public CS230::Component
{
public:
    Gravity(double value) : gravity(value)
    {
    }

    double GetValue()
    {
        return gravity;
    }

private:
    double gravity;
};
