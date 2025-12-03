/**
 * \file
 * \author Rudy Castan
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
/*
 * Timer.hpp
 * Provides both a util::Timer used by the Engine for delta timing and
 * a CS230::Timer component used by game objects.
 */
#pragma once

#include "Component.hpp"
#include <algorithm>
#include <chrono>

namespace util
{
    class [[nodiscard]] Timer
    {
    private:
        using clock_t  = std::chrono::steady_clock;
        using second_t = std::chrono::duration<double, std::ratio<1>>;

        std::chrono::time_point<clock_t> timeStamp;

    public:
        Timer() noexcept : timeStamp(clock_t::now())
        {
        }

        void ResetTimeStamp() noexcept
        {
            timeStamp = clock_t::now();
        }

        double GetElapsedSeconds() const noexcept
        {
            return std::chrono::duration_cast<second_t>(clock_t::now() - timeStamp).count();
        }
    };
}

namespace CS230
{
    class Timer : public Component
    {
    public:
        explicit Timer(double start_seconds = 0.0) : remaining(start_seconds), tick(false), accumulator(0.0), blink_period(0.25)
        {
        }

        void Set(double seconds)
        {
            remaining = seconds;
            accumulator = 0.0;
            tick = false;
        }

        void SetBlinkPeriod(double seconds)
        {
            blink_period = seconds;
        }

        double Remaining() const
        {
            return remaining;
        }

        bool TickTock() const
        {
            return tick;
        }

        void Update(double dt) override
        {
            if (remaining > 0.0)
            {
                remaining = std::max(0.0, remaining - dt);
                accumulator += dt;
                if (accumulator >= blink_period)
                {
                    accumulator -= blink_period;
                    tick = !tick;
                }
            }
        }

    private:
        double remaining;
        mutable bool tick;
        double accumulator;
        double blink_period;
    };
}
