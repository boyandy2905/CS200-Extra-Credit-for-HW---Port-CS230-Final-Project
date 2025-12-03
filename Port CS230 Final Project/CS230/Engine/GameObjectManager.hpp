/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  GameObjectManager.h
Project:    CS230 Engine
Author:     Jonathan Holmes, Sehoon Kim
Created:    April 19, 2025
*/

#pragma once
#include "Engine/Component.hpp"
#include "GameObject.hpp"
#include "Matrix.hpp"
#include <list>
#include <vector>

namespace Math
{
    class TransformationMatrix;
}

namespace CS230
{
    class GameObjectManager : public Component
    {
    public:
        void Add(GameObject* object);
        void UpdateAll(double dt);
        void DrawAll(Math::TransformationMatrix camera_matrix);
        void Unload();

        void CollisionTest();

    private:
        std::list<GameObject*> objects;
    };
}
