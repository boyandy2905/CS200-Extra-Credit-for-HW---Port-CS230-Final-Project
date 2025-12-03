/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Score.h
Project:    CS230 Game
Author:     Jonathan Holmes, Sehoon Kim
Created:    May 29, 2025
*/

#pragma once
#include "Component.hpp"

namespace CS230
{
    class Score : public Component
    {
    public:
        void Add(int value)
        {
            score += value;
        }

        int Value()
        {
            return score;
        }

    private:
        int score = 0;
    };
}
