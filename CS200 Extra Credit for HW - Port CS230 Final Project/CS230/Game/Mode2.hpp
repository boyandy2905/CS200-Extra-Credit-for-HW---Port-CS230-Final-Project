/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Mode2.h
Project:    CS230 Game
Author:     Jonathan Holmes, Sehoon Kim
Created:    March 13, 2025
*/

#pragma once
#include "Engine/GameObjectManager.hpp"
#include "Engine/GameState.hpp"
#include "Engine/Particle.hpp"
#include "Engine/ShowCollision.hpp"
#include "Engine/Sprite.hpp"
#include "Engine/Texture.hpp"
#include <memory>
#include "Particles.hpp"
 

class Ship;

class Mode2 : public CS230::GameState
{
public:
    Mode2();
    void Load() override;
    void Update(double dt) override;
    void Unload() override;
    void Draw() override;

    std::string GetName() override
    {
        return "Mode2";
    }

private:
    std::shared_ptr<CS230::Texture> texture;

    Ship*           ship;
    std::shared_ptr<CS230::Texture> restart;
    std::shared_ptr<CS230::Texture> gameover;

    unsigned int color = 0xFFFFFFFF;

    static constexpr double meteor_timer = 4.5;
    double                  last_time    = 0.0;

    std::shared_ptr<CS230::Texture> score_texture;
    int             score;
    void            update_score_text(int value);
};
