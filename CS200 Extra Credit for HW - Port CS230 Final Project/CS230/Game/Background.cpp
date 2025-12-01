/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Background.cpp
Project:    CS230 Game
Author:     Jonathan Holmes, Sehoon Kim
Created:    March 30, 2025
*/

#include "Background.hpp"
#include "Engine/Engine.hpp"
#include "Engine/TextureManager.hpp"
#include "Mode1.hpp"

void Background::Add(const std::filesystem::path& texture_path, double speed)
{
    backgrounds.push_back(ParallaxLayer{ Engine::GetTextureManager().Load(texture_path), speed });
}

void Background::Unload()
{
    backgrounds.clear();
}

void Background::Draw(const CS230::Camera& camera)
{
    for (ParallaxLayer& background : backgrounds)
    {
        Math::vec2              inverted_position = -Math::vec2{ camera.GetPosition().x * background.speed, camera.GetPosition().y };
        Math::TranslationMatrix new_matrix        = Math::TranslationMatrix(inverted_position);
        background.texture->Draw(new_matrix);
    }
}

Math::ivec2 Background::GetSize()
{
    return backgrounds[backgrounds.size() - 1].texture->GetSize();
}
