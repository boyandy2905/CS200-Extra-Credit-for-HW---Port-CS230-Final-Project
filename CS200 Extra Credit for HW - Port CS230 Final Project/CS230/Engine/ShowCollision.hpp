/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  ShowCollision.h
Project:    CS230 Engine
Author:     Jonathan Holmes, Sehoon Kim
Created:    May 17, 2025
*/

#pragma once
#include "Component.hpp"
#include "Engine.hpp"

namespace CS230
{
    class ShowCollision : public Component
    {
    public:
        ShowCollision();
        void Update(double dt) override;
        bool Enabled();
        void SetEnabled(bool e);
        std::shared_ptr<CS230::Texture> GetStatusTexture() const { return status_texture; }

    private:
        bool enabled;
        std::shared_ptr<CS230::Texture> status_texture;
    };
}
