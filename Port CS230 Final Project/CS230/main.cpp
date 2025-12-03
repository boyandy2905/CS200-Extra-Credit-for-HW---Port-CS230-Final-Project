/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  main.cpp
Project:    CS230 Engine
Author:     Jonathan Holmes, Sehoon Kim
Created:    March 6, 2025
*/

#include <iostream>
#include "Engine/Engine.hpp"
#include "Engine/Vec2.hpp"
#include "Game/Splash.hpp"
#include "Game/MainMenu.hpp"
#include "Game/Mode1.hpp"
#include "Game/Mode2.hpp"
#include "Game/Mode3.hpp"
#include <csignal>

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#endif
// execinfo.h (backtrace) is not available on Emscripten/Web builds.
#if !defined(__EMSCRIPTEN__)
#include <execinfo.h>
#include <unistd.h>

static void my_terminate_handler()
{
    void* arr[50];
    int size = backtrace(arr, 50);
    backtrace_symbols_fd(arr, size, STDERR_FILENO);
    std::_Exit(128);
}
#else
static void my_terminate_handler()
{
    // Simple terminate handler for web builds — no native backtrace available.
    std::cerr << "terminate called (no backtrace available on web)\n";
    std::_Exit(128);
}
#endif

int main() {
    try {
        std::set_terminate(my_terminate_handler);
        std::cerr << "[main] terminate handler installed\n";
        Engine& engine = Engine::Instance();
        std::cerr << "[main] Engine instance acquired\n";
        engine.Start("Final Project - sehoon.kim@digipen.edu & chanwoong.moon@digipen.edu");
        std::cerr << "[main] Engine started\n";

        engine.AddFont("Assets/Font_Simple.png");
        std::cerr << "[main] Added Font_Simple.png\n";
        engine.AddFont("Assets/Font_Outlined.png");
        std::cerr << "[main] Added Font_Outlined.png\n";

        // Create a global ShowCollision component so Tab toggles work from menus
        engine.CreateGlobalShowCollision();

        engine.GetGameStateManager().PushState<Splash>();
        std::cerr << "[main] Pushed Splash\n";

        std::cerr << "[main] Entering main loop\n";

#if defined(__EMSCRIPTEN__)
        static Engine* g_engine_ptr = nullptr;
        g_engine_ptr = &engine;

        auto MainLoopForEmscripten = [](void) {
            Engine& eng = *g_engine_ptr;
            if (eng.HasGameEnded()) {
                eng.Stop();
                emscripten_cancel_main_loop();
                return;
            }
            eng.Update();
        };

        emscripten_set_main_loop(MainLoopForEmscripten, 0, true);

        // When using emscripten_set_main_loop the function does not return
        // until emscripten_cancel_main_loop is called. After cancelation the
        // runtime may continue to run, but we'll return from main here.
        return 0;
#else
        while (engine.HasGameEnded() == false) {
            engine.Update();
        }

        std::cerr << "[main] Exited main loop\n";

        engine.Stop();
        std::cerr << "[main] Engine stopped\n";

        return 0;
#endif
    }
    catch (std::exception& e) {
        std::cerr << e.what() << "\n";
        return -1;
    }
}
