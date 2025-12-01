/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Collision.cpp
Project:    CS230 Engine
Author:     Jonathan Holmes, Sehoon Kim
Created:    May 17, 2025
*/

#include "Collision.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Window.hpp"
#include "Engine/Logger.hpp"
#include "CS200/IRenderer2D.hpp"
#include "CS200/RGBA.hpp"
#include <cmath>

// Local PI constant (avoid reliance on non-standard M_PI)
[[maybe_unused]] static constexpr double PI = 3.14159265358979323846;

// Rect

CS230::RectCollision::RectCollision(Math::irect boundary_in, GameObject* object_in) : object(object_in), boundary(boundary_in)
{
}

void CS230::RectCollision::Draw(Math::TransformationMatrix display_matrix)
{
    // Compute world-space AABB corners and draw using the renderer's transform
    Math::rect world_boundary = WorldBoundary();

    Math::vec2 bl = Math::vec2{ world_boundary.Left(), world_boundary.Bottom() };
    Math::vec2 br = Math::vec2{ world_boundary.Right(), world_boundary.Bottom() };
    Math::vec2 tl = Math::vec2{ world_boundary.Left(), world_boundary.Top() };
    Math::vec2 tr = Math::vec2{ world_boundary.Right(), world_boundary.Top() };

    // Use renderer's world-space line draw (apply camera/display transform)
    Engine::GetRenderer2D().DrawLine(display_matrix, tl, tr, CS200::WHITE, 1.0);
    Engine::GetRenderer2D().DrawLine(display_matrix, br, tr, CS200::WHITE, 1.0);
    Engine::GetRenderer2D().DrawLine(display_matrix, br, bl, CS200::WHITE, 1.0);
    Engine::GetRenderer2D().DrawLine(display_matrix, tl, bl, CS200::WHITE, 1.0);
}

bool CS230::RectCollision::IsCollidingWith(GameObject* other_object)
{
    Collision* other_collider = other_object->GetGOComponent<Collision>();

    if (other_collider == nullptr)
    {
        return false;
    }
    if (other_collider->Shape() != CollisionShape::Rect)
    {
        Engine::GetLogger().LogError("Rect vs unsupported type");
        return false;
    }

    Math::rect rectangle_1 = WorldBoundary();
    Math::rect rectangle_2 = dynamic_cast<RectCollision*>(other_collider)->WorldBoundary();

    if (rectangle_1.Left() < rectangle_2.Right() && rectangle_1.Right() > rectangle_2.Left() && rectangle_1.Bottom() < rectangle_2.Top() && rectangle_1.Top() > rectangle_2.Bottom())
    {
        return true;
    }
    return false;
}

bool CS230::RectCollision::IsCollidingWith(Math::vec2 point)
{
    Math::rect rectangle = WorldBoundary();

    if (rectangle.Left() <= point.x && rectangle.Right() >= point.x && rectangle.Bottom() <= point.y && rectangle.Top() >= point.y)
    {
        return true;
    }
    return false;
}

Math::rect CS230::RectCollision::WorldBoundary()
{
    // Construct the axis-aligned bounding box in world coordinates by
    // transforming all four local rectangle corners and taking min/max.
    double x1 = static_cast<double>(boundary.Left());
    double x2 = static_cast<double>(boundary.Right());
    double y1 = static_cast<double>(boundary.Bottom());
    double y2 = static_cast<double>(boundary.Top());

    Math::vec2 local_corners[4] = {
        Math::vec2{x1, y1}, // bottom-left
        Math::vec2{x2, y1}, // bottom-right
        Math::vec2{x1, y2}, // top-left
        Math::vec2{x2, y2}  // top-right
    };

    Math::vec2 world_corners[4];
    for (int i = 0; i < 4; ++i)
        world_corners[i] = object->GetMatrix() * local_corners[i];

    double min_x = world_corners[0].x;
    double max_x = world_corners[0].x;
    double min_y = world_corners[0].y;
    double max_y = world_corners[0].y;

    for (int i = 1; i < 4; ++i)
    {
        if (world_corners[i].x < min_x) min_x = world_corners[i].x;
        if (world_corners[i].x > max_x) max_x = world_corners[i].x;
        if (world_corners[i].y < min_y) min_y = world_corners[i].y;
        if (world_corners[i].y > max_y) max_y = world_corners[i].y;
    }

    Math::rect result;
    result.point_1 = Math::vec2{ min_x, min_y };
    result.point_2 = Math::vec2{ max_x, max_y };
    return result;
}

// Circle

CS230::CircleCollision::CircleCollision(double radius_in, GameObject* object_in) : object(object_in), radius(radius_in)
{
}

void CS230::CircleCollision::Draw(Math::TransformationMatrix display_matrix)
{
    // Draw a circle using the renderer's SDF system. The renderer expects the
    // transform to encode the world-space size of the shape. Compute a
    // transform that translates to the object's position and scales the unit
    // quad to the collision diameter (2 * radius). Rotation is irrelevant for
    // circles so we omit it.
    const double radius_world = GetRadius();
    const double diameter     = radius_world * 2.0;
    Math::TransformationMatrix world_transform = display_matrix * Math::TranslationMatrix(object->GetPosition()) * Math::ScaleMatrix(Math::vec2{ diameter, diameter });
    Engine::GetRenderer2D().DrawCircle(world_transform, CS200::CLEAR, CS200::WHITE, 1.0);
}

bool CS230::CircleCollision::IsCollidingWith(GameObject* other_object)
{
    Collision* other_collider = other_object->GetGOComponent<Collision>();

    if (other_collider == nullptr)
    {
        return false;
    }
    if (other_collider->Shape() != CollisionShape::Circle)
    {
        Engine::GetLogger().LogError("Circle vs unsupported type");
        return false;
    }

    double circle_1 = GetRadius();
    double circle_2 = dynamic_cast<CircleCollision*>(other_collider)->GetRadius();

    Math::vec2 pos_1  = object->GetPosition();
    Math::vec2 pos_2  = other_object->GetPosition();
    double     diff_x = pos_1.x - pos_2.x;
    double     diff_y = pos_1.y - pos_2.y;

    if (diff_x * diff_x + diff_y * diff_y < (circle_1 + circle_2) * (circle_1 + circle_2))
    {
        return true;
    }
    return false;
}

bool CS230::CircleCollision::IsCollidingWith(Math::vec2 point)
{
    Math::vec2 pos    = object->GetPosition();
    double     circle = GetRadius();

    double diff_x = pos.x - point.x;
    double diff_y = pos.y - point.y;

    if (diff_x * diff_x + diff_y * diff_y <= circle * circle)
    {
        return true;
    }
    return false;
}

double CS230::CircleCollision::GetRadius()
{
    return radius * (object->GetScale().x + object->GetScale().y) / 2.0;
}
