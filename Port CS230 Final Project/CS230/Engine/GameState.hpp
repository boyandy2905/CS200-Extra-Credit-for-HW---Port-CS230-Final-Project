/* GameState.hpp - per-state component helpers */
#pragma once

#include <gsl/gsl>
#include <string>
#include "Component.hpp"
#include <vector>
#include <memory>

namespace CS230
{
    class GameState
    {
    public:
        virtual void          Load()          = 0;
        virtual void          Update(double dt)        = 0;
        virtual void          Unload()        = 0;
        virtual void          Draw()        = 0;
        virtual void          DrawImGui()     {}
        virtual std::string   GetName() = 0;
        virtual ~GameState()                  = default;

        void AddGSComponent(Component* component)
        {
            mGSComponents.emplace_back(component);
        }

        template <typename T>
        T* GetGSComponent()
        {
            for (auto& c : mGSComponents)
            {
                if (auto p = dynamic_cast<T*>(c.get()))
                    return p;
            }
            return nullptr;
        }

        void UpdateGSComponents(double dt)
        {
            for (auto& c : mGSComponents)
                c->Update(dt);
        }

        void ClearGSComponents()
        {
            mGSComponents.clear();
        }

    private:
        std::vector<std::unique_ptr<Component>> mGSComponents;
    };
}
