/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  MainMenu.cpp
Project:    CS230 Game
Author:     Jonathan Holmes, Sehoon Kim
Created:    May 6, 2025
*/

#include "MainMenu.hpp"
#include "../Engine/Window.hpp"
#include "../Engine/Input.hpp"
#include "../Engine/GameStateManager.hpp"
#include "../Engine/TextureManager.hpp"
#include <cmath>
#include "../Engine/Font.hpp"
#include "Engine/Engine.hpp"
#include "Fonts.hpp"
#include "States.hpp"

MainMenu::MainMenu()
{
}

void MainMenu::Load()
{
    // Use Engine's font API (ensure correct include). If Engine::GetFont is not available,
    // this will be adjusted after locating font API. For now keep call but require proper header.
    mainmenu = Engine::GetFont(static_cast<int>(Fonts::Outlined)).PrintToTexture("CS230 Engine Test", 0xA181F7FF);

    // When entering the main menu, ensure the global collision-box display is turned off
    if (Engine::GetGlobalShowCollision())
    {
        Engine::GetGlobalShowCollision()->SetEnabled(false);
    }

    mode1_sel = Engine::GetFont(static_cast<int>(Fonts::Outlined)).PrintToTexture("Side Scroller", color1);
    mode2_sel = Engine::GetFont(static_cast<int>(Fonts::Outlined)).PrintToTexture("Space Shooter", color1);
    mode3_sel = Engine::GetFont(static_cast<int>(Fonts::Outlined)).PrintToTexture("Final Project", color1);
    exit_sel  = Engine::GetFont(static_cast<int>(Fonts::Outlined)).PrintToTexture("Exit", color1);

    mode1_un = Engine::GetFont(static_cast<int>(Fonts::Outlined)).PrintToTexture("Side Scroller", color2);
    mode2_un = Engine::GetFont(static_cast<int>(Fonts::Outlined)).PrintToTexture("Space Shooter", color2);
    mode3_un = Engine::GetFont(static_cast<int>(Fonts::Outlined)).PrintToTexture("Final Project", color2);
    exit_un  = Engine::GetFont(static_cast<int>(Fonts::Outlined)).PrintToTexture("Exit", color2);
}

void MainMenu::Update([[maybe_unused]] double dt)
{
    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Enter))
    {
        if (mode1_b == true && mode2_b == false && mode3_b == false && exit_b == false)
        {
            Engine::GetGameStateManager().RequestNextGameState(static_cast<int>(States::Mode1));
        }
        else if (mode1_b == false && mode2_b == true && mode3_b == false && exit_b == false)
        {
            Engine::GetGameStateManager().RequestNextGameState(static_cast<int>(States::Mode2));
        }
        else if (mode1_b == false && mode2_b == false && mode3_b == true && exit_b == false)
        {
            Engine::GetGameStateManager().RequestNextGameState(static_cast<int>(States::Mode3));
        }
        else if (mode1_b == false && mode2_b == false && mode3_b == false && exit_b == true)
        {
            // Clear all game states so the engine main loop will exit
            Engine::GetLogger().LogEvent("MainMenu: Exit selected, clearing all game states");
            Engine::GetGameStateManager().Clear();
            return; // avoid further access after clearing states
        }
    }
    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Up))
    {
        if (mode1_b == true && mode2_b == false && mode3_b == false && exit_b == false)
        {
            mode1_b = false;
            exit_b  = true;
        }
        else if (mode1_b == false && mode2_b == true && mode3_b == false && exit_b == false)
        {
            mode2_b = false;
            mode1_b = true;
        }
        else if (mode1_b == false && mode2_b == false && mode3_b == true && exit_b == false)
        {
            mode3_b = false;
            mode2_b = true;
        }
        else if (mode1_b == false && mode2_b == false && mode3_b == false && exit_b == true)
        {
            exit_b  = false;
            mode3_b = true;
        }
    }
    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Down))
    {
        if (mode1_b == true && mode2_b == false && mode3_b == false && exit_b == false)
        {
            mode1_b = false;
            mode2_b = true;
        }
        else if (mode1_b == false && mode2_b == true && mode3_b == false && exit_b == false)
        {
            mode2_b = false;
            mode3_b = true;
        }
        else if (mode1_b == false && mode2_b == false && mode3_b == true && exit_b == false)
        {
            mode3_b = false;
            exit_b  = true;
        }
        else if (mode1_b == false && mode2_b == false && mode3_b == false && exit_b == true)
        {
            exit_b  = false;
            mode1_b = true;
        }
    }
}

void MainMenu::Unload()
{
    mainmenu.reset();
    mode1_sel.reset();
    mode2_sel.reset();
    mode3_sel.reset();
    exit_sel.reset();
    mode1_un.reset();
    mode2_un.reset();
    mode3_un.reset();
    exit_un.reset();
}

void MainMenu::Draw()
{
    Engine::GetWindow().Clear(0x252439FF);

    mainmenu->Draw(Math::TranslationMatrix(Math::ivec2(Engine::GetWindow().GetSize().x / 2 - 345, Engine::GetWindow().GetSize().y - 150)) * Math::ScaleMatrix(Math::vec2{ 1.5, 1.5 }));
    if (mode1_b)
    {
        mode1_sel->Draw(Math::TranslationMatrix(Math::ivec2(Engine::GetWindow().GetSize().x / 2 - 165, Engine::GetWindow().GetSize().y - 250)));
    }
    else
    {
        mode1_un->Draw(Math::TranslationMatrix(Math::ivec2(Engine::GetWindow().GetSize().x / 2 - 165, Engine::GetWindow().GetSize().y - 250)));
    }
    if (mode2_b)
    {
        mode2_sel->Draw(Math::TranslationMatrix(Math::ivec2(Engine::GetWindow().GetSize().x / 2 - 180, Engine::GetWindow().GetSize().y - 350)));
    }
    else
    {
        mode2_un->Draw(Math::TranslationMatrix(Math::ivec2(Engine::GetWindow().GetSize().x / 2 - 180, Engine::GetWindow().GetSize().y - 350)));
    }
    if (mode3_b)
    {
        mode3_sel->Draw(Math::TranslationMatrix(Math::ivec2(Engine::GetWindow().GetSize().x / 2 - 165, Engine::GetWindow().GetSize().y - 450)));
    }
    else
    {
        mode3_un->Draw(Math::TranslationMatrix(Math::ivec2(Engine::GetWindow().GetSize().x / 2 - 165, Engine::GetWindow().GetSize().y - 450)));
    }
    if (exit_b)
    {
        exit_sel->Draw(Math::TranslationMatrix(Math::ivec2(Engine::GetWindow().GetSize().x / 2 - 45, Engine::GetWindow().GetSize().y - 550)));
    }
    else
    {
        exit_un->Draw(Math::TranslationMatrix(Math::ivec2(Engine::GetWindow().GetSize().x / 2 - 45, Engine::GetWindow().GetSize().y - 550)));
    }
}
