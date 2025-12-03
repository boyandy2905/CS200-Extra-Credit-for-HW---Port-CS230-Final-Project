/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Background.h
Project:    CS230 Game
Author:     Jonathan Holmes, Sehoon Kim
Created:    March 30, 2025
*/

#pragma once
#include "Engine/Camera.hpp"
#include "Engine/Component.hpp"
#include "Engine/Texture.hpp"
#include <vector>
#include <memory>

class Background : public CS230::Component
{
public:
    void        Add(const std::filesystem::path& texture_path, double speed);
    void        Unload();
    void        Draw(const CS230::Camera& camera);
    void        SetSize(Math::ivec2 size);
    Math::ivec2 GetSize();

private:
    struct ParallaxLayer
    {
        std::shared_ptr<CS230::Texture> texture;
        double          speed = 1;
    };

    std::vector<ParallaxLayer> backgrounds;
    Math::ivec2 target_size { 0, 0 };
};
