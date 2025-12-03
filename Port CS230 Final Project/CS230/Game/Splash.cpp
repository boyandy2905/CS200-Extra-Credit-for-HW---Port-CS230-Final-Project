/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Splash.cpp
Project:    CS230 Game
Author:     Jonathan Holmes, Sehoon Kim
Created:    March 13, 2025
*/

#include "Splash.hpp"
#include "Engine/Engine.hpp"
#include "Engine/TextureManager.hpp"
#include "Engine/Logger.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Window.hpp"
#include "States.hpp"

Splash::Splash()
{
}

void Splash::Load()
{
    counter = 0;
    texture = Engine::GetTextureManager().Load("Assets/DigiPen.png");
    if (texture)
    {
        [[maybe_unused]] auto sz = texture->GetSize();
        // Engine::GetLogger().LogDebug("Loaded Splash texture: " + std::to_string(sz.x) + "x" + std::to_string(sz.y));
    }
}

void Splash::Update(double dt)
{
    counter += dt;
    if (counter >= 2.0)
    {
        Engine::GetGameStateManager().RequestNextGameState(static_cast<int>(States::MainMenu));
    }
}

void Splash::Unload()
{
    texture.reset();
}

void Splash::Draw()
{
    // Clear to white so areas not covered by the splash image are white
    Engine::GetWindow().Clear(0xFFFFFFFF);
    if (texture)
    {
        Math::vec2 center = (Math::vec2(Engine::GetWindow().GetSize()) - Math::vec2(texture->GetSize())) / 2.0;
        texture->Draw(Math::TranslationMatrix(center));
    }
}
