/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Portal.cpp
Project:    CS230 Game
Author:     Jonathan Holmes, Sehoon Kim
Created:    May 27, 2025
*/

#include "Portal.hpp"
#include "Engine/Engine.hpp"
#include "Mode1.hpp"

Portal::Portal(int dest_state, Math::irect boundary) : GameObject(static_cast<Math::vec2>(boundary.point_1)), to_state(dest_state)
{
    AddGOComponent(new CS230::RectCollision({Math::ivec2{ 0, 0 }, boundary.Size()}, this));
}

void Portal::GoToState()
{
    // Request the state change to be processed after the current update loop
    // finishes. Calling SetNextGameState directly here can cause the active
    // game state to be unloaded while collision resolution or update loops
    // are still iterating, which can lead to crashes.
    Engine::GetGameStateManager().RequestNextGameState(to_state);
}
