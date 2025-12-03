/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Mode3.h
Project:    CS230 Game
Author:     Sehoon Kim, Chanwoong Moon
Created:    June 14, 2025
*/

#pragma once
#include <vector>
#include "Engine/GameState.hpp"
#include "Engine/Texture.hpp"
#include <memory>
#include "Engine/Sprite.hpp"
#include "Background.hpp"
#include "Engine/GameObjectManager.hpp"
#include "Engine/Particle.hpp" 
#include "Engine/Timer.hpp"

class Player;

class Mode3 : public CS230::GameState {
public:
    Mode3();
    void Load() override;
    void Update(double dt) override;
    void Unload() override;
    void Draw() override;

    std::string GetName() override {
        return "Mode3";
    }

    static constexpr double timer_max = 40;

private:
    std::shared_ptr<CS230::Texture> texture;
    Player* player_ptr = nullptr;
    static constexpr double car_timer = 4.5;
    [[maybe_unused]] double last_time = 0.0;

    std::shared_ptr<CS230::Texture> timer_texture = nullptr;
    int last_timer = 0;
    void update_timer_text(int value);
    std::shared_ptr<CS230::Texture> distance_texture = nullptr;
    double total_distance = 0.0;
    int last_distance = 0;
    void update_distance_text(int distance);
};
