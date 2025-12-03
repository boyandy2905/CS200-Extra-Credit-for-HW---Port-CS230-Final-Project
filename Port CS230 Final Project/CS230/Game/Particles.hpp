/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Particles.h
Project:    CS230 Engine
Author:     Jonathan Holmes, Sehoon Kim
Created:    June 7, 2025
*/

#pragma once
#include "Engine/Particle.hpp"

namespace Particles
{
    class Smoke : public CS230::Particle
    {
    public:
        Smoke() : Particle("Assets/Smoke.spt") { };
        std::string TypeName() override
        {
            return "Smoke Particle";
        }
        static constexpr int    MaxCount = 3;
        static constexpr double MaxLife  = 5.0;
    };

    class Hit : public CS230::Particle
    {
    public:
        Hit() : Particle("Assets/Hit.spt") { };
        std::string TypeName() override
        {
            return "Hit Particle";
        }
        static constexpr int    MaxCount = 10;
        static constexpr double MaxLife  = 1.0;
    };

    class MeteorBit : public CS230::Particle
    {
    public:
        MeteorBit() : Particle("Assets/MeteorBit.spt") { };
        std::string TypeName() override
        {
            return "MeteorBit Particle";
        }
        static constexpr int    MaxCount = 150;
        static constexpr double MaxLife  = 1.25;
    };

    class Bump : public CS230::Particle
    {
    public:
        Bump() : Particle("Assets/Bump.spt") { };
        std::string TypeName() override
        {
            return "Bump Particle";
        }
        static constexpr int    MaxCount = 150;
        static constexpr double MaxLife  = 1.25;
    };

    class Exhaust : public CS230::Particle
    {
    public:
        Exhaust() : Particle("Assets/Exhaust.spt") { };
        std::string TypeName() override
        {
            return "Exhaust Particle";
        }
        static constexpr int    MaxCount = 150;
        static constexpr double MaxLife  = 1.25;
    };
}
