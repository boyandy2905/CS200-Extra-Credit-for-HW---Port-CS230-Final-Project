/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Mode1.h
Project:    CS230 Game
Author:     Jonathan Holmes, Sehoon Kim
Created:    March 13, 2025
*/

#pragma once
#include "Engine/GameObjectManager.hpp"
#include "Engine/GameState.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Particle.hpp"
#include "Engine/ShowCollision.hpp"
#include "Engine/Sprite.hpp"
#include "Engine/Texture.hpp"
#include <memory>
#include "Engine/Timer.hpp"
#include "Background.hpp"
#include "Gravity.hpp"
#include "Particles.hpp"
 

class Cat;
class Floor;

class Mode1 : public CS230::GameState
{
public:
    Mode1();
    void Load() override;
    void Update(double dt) override;
    void Unload() override;
    void Draw() override;

    std::string GetName() override
    {
        return "Mode1";
    }

    static constexpr double gravity = 800;
    static constexpr double floor   = 80;

    static constexpr double timer_max = 60;

private:
    std::shared_ptr<CS230::Texture> texture;
    Cat*           cat_ptr;
    Floor*         starting_floor_ptr;

    std::shared_ptr<CS230::Texture> timer_texture;
    int             last_timer;
    void            update_timer_text(int value);

    std::shared_ptr<CS230::Texture> score_texture;
    int             score;
    void            update_score_text(int value);
};
