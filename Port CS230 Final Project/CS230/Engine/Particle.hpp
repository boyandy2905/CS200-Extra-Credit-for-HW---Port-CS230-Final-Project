/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Particle.h
Project:    CS230 Engine
Author:     Jonathan Holmes, Sehoon Kim
Created:    June 7, 2025
*/

#pragma once
#include "Engine/GameStateManager.hpp"
#include "Engine/Logger.hpp"
#include "Game/GameObjectTypes.hpp"
#include "GameObject.hpp"
#include "GameObjectManager.hpp"
#include "Vec2.hpp"
#include <cstdlib>

namespace CS230
{
    class Particle : public GameObject
    {
    public:
        Particle(const std::filesystem::path& sprite_file);
        void Start(Math::vec2 new_position, Math::vec2 new_velocity, double max_life);
        void Update(double dt) override;
        void Draw(Math::TransformationMatrix camera_matrix) override;

        bool Alive()
        {
            return life > 0;
        }

        GameObjectTypes Type() override
        {
            return GameObjectTypes::Particle;
        }

    private:
        double life;
    };

    template <typename T>
    class ParticleManager : public Component
    {
    public:
        ParticleManager();
        ~ParticleManager();
        void Emit(int count, Math::vec2 emitter_position, Math::vec2 emitter_velocity, Math::vec2 direction, double spread);

    private:
        std::vector<T*> particles;
        std::size_t     index;
    };
}

namespace CS230
{

    template <typename T>
    ParticleManager<T>::ParticleManager() : index(0)
    {
        for (std::size_t i = 0; i < T::MaxCount; ++i)
        {
            T* ptc = new T();
            Engine::GetGameStateManager().GetGSComponent<GameObjectManager>()->Add(ptc);
            particles.push_back(ptc);
        }
    }

    template <typename T>
    ParticleManager<T>::~ParticleManager()
    {
        particles.clear();
    }

    template <typename T>
    void ParticleManager<T>::Emit(int count, Math::vec2 emitter_position, Math::vec2 emitter_velocity, Math::vec2 direction, double spread)
    {
        if (particles.empty())
        {
            Engine::GetLogger().LogError("ParticleManager: no particles to emit");
            return;
        }

        for (int i = 0; i < count; ++i)
        {
            if (particles[index] == nullptr)
            {
                Engine::GetLogger().LogError("ParticleManager: null particle slot");
            }
            else if (particles[index]->Alive())
            {
                
            }

            double angle_variation = 0.0;
            if (spread != 0.0)
            {
                angle_variation = ((rand() % static_cast<int>(spread * 1024)) / 1024.0) - spread / 2.0;
            }

            Math::vec2 random_magnitude  = direction * (((rand() % 1024) / 2048.0) + 0.5);
            Math::vec2 particle_velocity = Math::RotationMatrix(angle_variation) * random_magnitude + emitter_velocity;
            if (particles[index] != nullptr)
            {
                particles[index]->Start(emitter_position, particle_velocity, T::MaxLife);
            }

            index++;
            if (index >= particles.size())
            {
                index = 0;
            }
        }
    }
} // namespace CS230
