/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Sehoon Kim
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "Vec2.hpp"
#include <algorithm>

namespace Math
{
    struct [[nodiscard]] rect
    {
        Math::vec2 point_1{ 0.0, 0.0 };
        Math::vec2 point_2{ 0.0, 0.0 };

        [[nodiscard]] Math::vec2 Size() const noexcept
        {
            return { Right() - Left(), std::abs(Top() - Bottom()) };
        }

        [[nodiscard]] double Left() const noexcept
        {
            return std::min(point_1.x, point_2.x);
        }

        [[nodiscard]] double Right() const noexcept
        {
            return std::max(point_1.x, point_2.x);
        }

        [[nodiscard]] double Bottom() const noexcept
        {
            return std::min(point_1.y, point_2.y);
        }

        [[nodiscard]] double Top() const noexcept
        {
            return std::max(point_1.y, point_2.y);
        }
    };

    struct [[nodiscard]] irect
    {
        Math::ivec2 point_1{ 0, 0 };
        Math::ivec2 point_2{ 0, 0 };

        [[nodiscard]] Math::ivec2 Size() const noexcept
        {
            return { Right() - Left(), std::abs(Top() - Bottom()) };
        }

        [[nodiscard]] int Left() const noexcept
        {
            return std::min(point_1.x, point_2.x);
        }

        [[nodiscard]] int Right() const noexcept
        {
            return std::max(point_1.x, point_2.x);
        }

        [[nodiscard]] int Bottom() const noexcept
        {
            return std::min(point_1.y, point_2.y);
        }

        [[nodiscard]] int Top() const noexcept
        {
            return std::max(point_1.y, point_2.y);
        }
    };
}
