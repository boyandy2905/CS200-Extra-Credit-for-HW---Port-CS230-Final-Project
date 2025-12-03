/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Cat.cpp
Project:    CS230 Game
Author:     Jonathan Holmes, Sehoon Kim
Created:    March 22, 2025
*/

#include "Cat.hpp"
#include "Engine/Engine.hpp"
#include "Mode1.hpp"
#include "Engine/Window.hpp"
#include <cmath>

// Local PI constant used in particle emission angles
static constexpr double PI = 3.14159265358979323846;
#include "States.hpp"

Cat::Cat(Math::vec2 start_position, GameObject* first_platform) : GameObject(start_position), start_y_position(start_position.y), standing_on(first_platform)
{
    AddGOComponent(new CS230::Sprite("Assets/Cat.spt", this));
    change_state(&state_idle);
    hurt_timer = new CS230::Timer(0.0);
    AddGOComponent(hurt_timer);
}

void Cat::Update(double dt)
{
    GameObject::Update(dt);
    CS230::RectCollision* collider = GetGOComponent<CS230::RectCollision>();
    // Boundary Check
    if (collider->WorldBoundary().Left() < Engine::GetGameStateManager().GetGSComponent<CS230::Camera>()->GetPosition().x)
    {
        UpdatePosition(Math::vec2{ Engine::GetGameStateManager().GetGSComponent<CS230::Camera>()->GetPosition().x - collider->WorldBoundary().Left(), 0 });
        SetVelocity({ 0, GetVelocity().y });
    }
    if (collider->WorldBoundary().Right() > Engine::GetGameStateManager().GetGSComponent<CS230::Camera>()->GetPosition().x + Engine::GetWindow().GetSize().x)
    {
        UpdatePosition(Math::vec2{ Engine::GetGameStateManager().GetGSComponent<CS230::Camera>()->GetPosition().x + Engine::GetWindow().GetSize().x - collider->WorldBoundary().Right(), 0 });
        SetVelocity({ 0, GetVelocity().y });
    }
}

void Cat::update_x_velocity(double dt)
{
    if (Engine::GetInput().KeyDown(CS230::Input::Keys::Right))
    {
        SetVelocity({ GetVelocity().x + x_acceleration * dt, GetVelocity().y });
        if (GetVelocity().x > max_velocity)
        {
            SetVelocity({ max_velocity, GetVelocity().y });
        }
    }
    else if (Engine::GetInput().KeyDown(CS230::Input::Keys::Left))
    {
        SetVelocity({ GetVelocity().x - x_acceleration * dt, GetVelocity().y });
        if (GetVelocity().x < -max_velocity)
        {
            SetVelocity({ -max_velocity, GetVelocity().y });
        }
    }
    else
    {
        if (GetVelocity().x > x_drag * dt)
        {
            SetVelocity({ GetVelocity().x - x_drag * dt, GetVelocity().y });
        }
        else if (GetVelocity().x < -x_drag * dt)
        {
            SetVelocity({ GetVelocity().x + x_drag * dt, GetVelocity().y });
        }
        else
        {
            SetVelocity({ 0, GetVelocity().y });
        }
    }
}

bool Cat::CanCollideWith(GameObjectTypes other_object_type)
{
    return other_object_type != GameObjectTypes::Particle;
}

void Cat::ResolveCollision(GameObject* other_object)
{
    Math::rect cat_rect   = GetGOComponent<CS230::RectCollision>()->WorldBoundary();
    Math::rect other_rect = other_object->GetGOComponent<CS230::RectCollision>()->WorldBoundary();

    switch (other_object->Type())
    {
        case GameObjectTypes::Floor: [[fallthrough]];
        case GameObjectTypes::Crates:
            if (current_state == &state_falling)
            {
                if (cat_rect.Top() > other_rect.Top())
                {
                    if (other_object->IsCollidingWith(GetPosition()))
                    {
                        SetPosition({ GetPosition().x, other_rect.Top() });
                        standing_on = other_object;
                        current_state->CheckExit(this);
                        return;
                    }
                }
            }
            if (cat_rect.Left() < other_rect.Left())
            {
                UpdatePosition(Math::vec2{ (other_rect.Left() - cat_rect.Right()), 0.0 });
                SetVelocity({ 0, GetVelocity().y });
            }
            else
            {
                UpdatePosition(Math::vec2{ (other_rect.Right() - cat_rect.Left()), 0.0 });
                SetVelocity({ 0, GetVelocity().y });
            }
            break;
        case GameObjectTypes::Robot:
            if (current_state == &state_falling && cat_rect.Top() > other_rect.Top() && hurt_timer->Remaining() == 0)
            {
                Engine::GetGameStateManager().GetGSComponent<CS230::ParticleManager<Particles::Smoke>>()->Emit(2, GetPosition(), { 0, 0 }, { 0, 100 }, 2 * PI / 3);
                SetPosition({ GetPosition().x, other_rect.Top() });
                SetVelocity({ GetVelocity().x, pop_velocity });
                other_object->ResolveCollision(this);
            }
            else
            {
                if (cat_rect.Left() < other_rect.Left())
                {
                    SetVelocity({ -hurt_acceleration, hurt_velocity });
                }
                else
                {
                    SetVelocity({ hurt_acceleration, hurt_velocity });
                }
                change_state(&state_falling);
                hurt_timer->SetBlinkPeriod(0.1);
                hurt_timer->Set(hurt_time);
            }
            standing_on = nullptr;
            break;
        case GameObjectTypes::Asteroid:
            if (current_state == &state_falling && cat_rect.Top() > other_rect.Top() && hurt_timer->Remaining() == 0)
            {
                SetPosition({ GetPosition().x, other_rect.Top() });
                SetVelocity({ GetVelocity().x, bounce_velocity });
                other_object->ResolveCollision(this);
            }
            else
            {
                if (cat_rect.Left() < other_rect.Left())
                {
                    SetVelocity({ -hurt_acceleration, hurt_velocity });
                }
                else
                {
                    SetVelocity({ hurt_acceleration, hurt_velocity });
                }
                change_state(&state_falling);
                hurt_timer->SetBlinkPeriod(0.1);
                hurt_timer->Set(hurt_time);
            }
            standing_on = nullptr;
            break;
        case GameObjectTypes::Portal:
            {
                static_cast<Portal*>(other_object)->GoToState();
                break;
                default:
                    break;
            }
    }
}

void Cat::Draw(Math::TransformationMatrix camera_matrix)
{
    if (hurt_timer->Remaining() == 0.0 || hurt_timer->TickTock() == true)
    {
        CS230::GameObject::Draw(camera_matrix);
    }
}

// Jumping
void Cat::State_Jumping::Enter(GameObject* object)
{
    Cat* cat = static_cast<Cat*>(object);

    cat->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Jumping));
    cat->UpdateVelocity({ 0, Cat::jump_velocity });
    cat->standing_on = nullptr;
}

void Cat::State_Jumping::Update(GameObject* object, double dt)
{
    Cat* cat = static_cast<Cat*>(object);

    {
        auto* gcomp = Engine::GetGameStateManager().GetGSComponent<Gravity>();
        double g_val = gcomp ? gcomp->GetValue() : 0.0;
        cat->SetVelocity({ cat->GetVelocity().x, cat->GetVelocity().y - g_val * dt });
    }
    cat->update_x_velocity(dt);
}

void Cat::State_Jumping::CheckExit(GameObject* object)
{
    Cat* cat = static_cast<Cat*>(object);

    if (Engine::GetInput().KeyDown(CS230::Input::Keys::Up) == false)
    {
        cat->change_state(&cat->state_falling);
        cat->SetVelocity({ cat->GetVelocity().x, 0 });
    }
    else if (cat->GetVelocity().y <= 0)
    {
        cat->change_state(&cat->state_falling);
    }
}

// Idle
void Cat::State_Idle::Enter(GameObject* object)
{
    Cat* cat = static_cast<Cat*>(object);

    cat->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Idle));
}

void Cat::State_Idle::Update([[maybe_unused]] GameObject* object, [[maybe_unused]] double dt)
{
}

void Cat::State_Idle::CheckExit(GameObject* object)
{
    Cat* cat = static_cast<Cat*>(object);

    if (Engine::GetInput().KeyDown(CS230::Input::Keys::Left))
    {
        cat->SetScale({ -1.0, 1.0 });
        cat->change_state(&cat->state_running);
    }
    else if (Engine::GetInput().KeyDown(CS230::Input::Keys::Right))
    {
        cat->SetScale({ 1.0, 1.0 });
        cat->change_state(&cat->state_running);
    }
    else if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Up))
    {
        cat->change_state(&cat->state_jumping);
    }
}

// Falling
void Cat::State_Falling::Enter(GameObject* object)
{
    Cat* cat = static_cast<Cat*>(object);

    cat->fall_start_y_position = cat->GetPosition().y;
    cat->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Falling));
}

void Cat::State_Falling::Update(GameObject* object, double dt)
{
    Cat* cat = static_cast<Cat*>(object);

    {
        auto* gcomp = Engine::GetGameStateManager().GetGSComponent<Gravity>();
        double g_val = gcomp ? gcomp->GetValue() : 0.0;
        cat->SetVelocity({ cat->GetVelocity().x, cat->GetVelocity().y - g_val * dt });
    }
    cat->update_x_velocity(dt);
}

void Cat::State_Falling::CheckExit(GameObject* object)
{
    Cat* cat = static_cast<Cat*>(object);

    if (cat->standing_on != nullptr)
    {
        double falling_height = cat->fall_start_y_position - cat->GetPosition().y;
        if (falling_height > LargeFallHeight)
        {
            Engine::GetGameStateManager().GetGSComponent<CS230::ParticleManager<Particles::Smoke>>()->Emit(2, cat->GetPosition(), { 0, 0 }, { 0, 100 }, PI / 2);
        }
        if (cat->GetVelocity().x > 0)
        {
            if (Engine::GetInput().KeyDown(CS230::Input::Keys::Left))
            {
                cat->change_state(&cat->state_skidding);
            }
            else
            {
                cat->change_state(&cat->state_running);
            }
        }
        else if (cat->GetVelocity().x < 0)
        {
            if (Engine::GetInput().KeyDown(CS230::Input::Keys::Right))
            {
                cat->change_state(&cat->state_skidding);
            }
            else
            {
                cat->change_state(&cat->state_running);
            }
        }
        else
        {
            cat->change_state(&cat->state_idle);
        }
        cat->SetVelocity({ cat->GetVelocity().x, 0 });
    }
    if (cat->GetPosition().y < -500)
    {
        // Schedule safe transition to MainMenu after this update loop.
        Engine::GetGameStateManager().RequestNextGameState(static_cast<int>(States::MainMenu));
    }
}

// Running
void Cat::State_Running::Enter(GameObject* object)
{
    Cat* cat = static_cast<Cat*>(object);

    cat->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Running));
    if (Engine::GetInput().KeyDown(CS230::Input::Keys::Right) && cat->GetVelocity().x > 0)
    {
        cat->SetScale({ 1.0, 1.0 });
    }
    else if (Engine::GetInput().KeyDown(CS230::Input::Keys::Left) && cat->GetVelocity().x < 0)
    {
        cat->SetScale({ -1.0, 1.0 });
    }
}

void Cat::State_Running::Update(GameObject* object, double dt)
{
    Cat* cat = static_cast<Cat*>(object);

    cat->update_x_velocity(dt);
}

void Cat::State_Running::CheckExit(GameObject* object)
{
    Cat* cat = static_cast<Cat*>(object);

    if (cat->GetVelocity().x == 0)
    {
        cat->change_state(&cat->state_idle);
    }
    else if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Up))
    {
        cat->change_state(&cat->state_jumping);
    }
    else if (cat->GetVelocity().x < 0 && Engine::GetInput().KeyDown(CS230::Input::Keys::Right))
    {
        cat->change_state(&cat->state_skidding);
    }
    else if (cat->GetVelocity().x >= 0 && Engine::GetInput().KeyDown(CS230::Input::Keys::Left))
    {
        cat->change_state(&cat->state_skidding);
    }

    if (cat->standing_on != nullptr && cat->standing_on->IsCollidingWith(cat->GetPosition()) == false)
    {
        cat->standing_on = nullptr;
        cat->change_state(&cat->state_falling);
    }
}

// Skidding
void Cat::State_Skidding::Enter(GameObject* object)
{
    Cat* cat = static_cast<Cat*>(object);

    cat->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::Skidding));

    Engine::GetGameStateManager().GetGSComponent<CS230::ParticleManager<Particles::Smoke>>()->Emit(2, cat->GetPosition(), { 0, 0 }, { 2 * cat->GetVelocity().x, 0 }, PI / 6);
}

void Cat::State_Skidding::Update(GameObject* object, double dt)
{
    Cat* cat = static_cast<Cat*>(object);

    if (cat->GetVelocity().x >= 0)
    {
        cat->SetVelocity({ cat->GetVelocity().x - (x_drag + x_acceleration) * dt, cat->GetVelocity().y });
    }
    else if (cat->GetVelocity().x < 0)
    {
        cat->SetVelocity({ cat->GetVelocity().x + (x_drag + x_acceleration) * dt, cat->GetVelocity().y });
    }
}

void Cat::State_Skidding::CheckExit(GameObject* object)
{
    Cat* cat = static_cast<Cat*>(object);

    if (cat->GetVelocity().x == 0)
    {
        cat->change_state(&cat->state_idle);
    }
    else if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Up))
    {
        cat->change_state(&cat->state_jumping);
    }
    else if (cat->GetVelocity().x < 0 && Engine::GetInput().KeyDown(CS230::Input::Keys::Left))
    {
        cat->change_state(&cat->state_running);
    }
    else if (cat->GetVelocity().x >= 0 && Engine::GetInput().KeyDown(CS230::Input::Keys::Right))
    {
        cat->change_state(&cat->state_running);
    }
}
