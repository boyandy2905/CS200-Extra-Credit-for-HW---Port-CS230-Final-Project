/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  OtherCarManager.cpp
Project:    CS230 Game
Author:     Sehoon Kim, Chanwoong Moon
Created:    June 14, 2025
*/

#include "OtherCarManager.hpp"
#include "Engine/Camera.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameObjectManager.hpp"
#include "Engine/Window.hpp"
#include "Mode3.hpp"
#include "OtherCar.hpp"

OtherCarManager::OtherCarManager(Mode3* mode) : CS230::GameObject({ 0, 0 }), spawn_timer(4.5), mode3_ptr(mode)
{
}

void OtherCarManager::Update(double dt)
{
    spawn_timer.Update(dt);

    if (spawn_timer.Remaining() <= 0.0)
    {
        std::vector<int> available_indices;
        for (size_t i = 0; i < y_positions.size(); ++i)
        {
            if (static_cast<int>(i) != last_used_index)
            {
                available_indices.push_back(static_cast<int>(i));
            }
        }

        if (!available_indices.empty())
        {
            int pos         = rand() % static_cast<int>(available_indices.size());
            int index       = available_indices[static_cast<std::size_t>(pos)];
            last_used_index = index;
            double y        = y_positions[static_cast<size_t>(index)];

            Math::vec2 camera_pos   = Engine::GetGameStateManager().GetGSComponent<CS230::Camera>()->GetPosition();
            double     screen_width = Engine::GetWindow().GetSize().x;
            Math::vec2 spawn_pos{ camera_pos.x + screen_width + 80.0, y };
            auto*      car = new OtherCar(spawn_pos);
            Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>()->Add(car);
        }

        spawn_timer.Set(0.5);
    }
}
