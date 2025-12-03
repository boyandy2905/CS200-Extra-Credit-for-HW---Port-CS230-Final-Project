/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "GameStateManager.hpp"

#include "../Game/Splash.hpp"
#include "../Game/MainMenu.hpp"
#include "../Game/Mode1.hpp"
#include "../Game/Mode2.hpp"
#include "../Game/Mode3.hpp"
#include "../Game/States.hpp"

namespace CS230
{
    void GameStateManager::PopState()
    {
        using namespace std::literals;
        auto* const state = mGameStateStack.back().get();
        mToClear.push_back(std::move(mGameStateStack.back()));
        mGameStateStack.erase(mGameStateStack.end() - 1);
        Engine::GetLogger().LogEvent("Exiting state "s + state->GetName());
        state->Unload();
    }

    void GameStateManager::Update()
    {
        mToClear.clear();
        // If no states are present, nothing to update
        if (mGameStateStack.empty())
            return;
        // Forward delta time from the Engine window environment to the active state
        mGameStateStack.back()->Update(Engine::GetWindowEnvironment().DeltaTime);

        // If a state transition was requested during the update (for example
        // by a collision handler), perform it now. This avoids tearing down
        // the currently-active state while its update/collision loop is
        // still iterating, which can lead to use-after-free / segfaults.
        if (mRequestedState != -1)
        {
            int req = mRequestedState;
            mRequestedState = -1;
            SetNextGameState(req);
        }
    }

    void GameStateManager::Draw()
    {
        for (auto& game_state : mGameStateStack)
        {
            game_state->Draw();
        }
    }

    void GameStateManager::DrawImGui()
    {
        if (mGameStateStack.empty())
            return;
        mGameStateStack.back()->DrawImGui();
    }

    void GameStateManager::Clear()
    {
        while (!mGameStateStack.empty())
            PopState();
        mToClear.clear();
    }

    void GameStateManager::SetNextGameState(int state_index)
    {
        // NOTE: This method performs a concrete state transition: it will pop
        // the current active state and push the requested new state type.
        // The mapping uses the project's `States` enum.
        using namespace std::literals;
        // If no state requested or stack empty, do nothing
        // The caller passes an int corresponding to States
        // We'll cast and switch to the concrete state push
        // Pop current state first if any
        if (!mGameStateStack.empty())
        {
            auto* state = mGameStateStack.back().get();
            PopState();
            (void)state;
        }

        switch (static_cast<States>(state_index))
        {
        case States::Splash:
            PushState<Splash>();
            break;
        case States::MainMenu:
            PushState<MainMenu>();
            break;
        case States::Mode1:
            PushState<Mode1>();
            break;
        case States::Mode2:
            PushState<Mode2>();
            break;
        case States::Mode3:
            PushState<Mode3>();
            break;
        default:
            // Unknown state; no-op
            break;
        }
    }

    void GameStateManager::RequestNextGameState(int state_index)
    {
        mRequestedState = state_index;
    }

    void GameStateManager::ClearNextGameState()
    {
        // No-op placeholder to satisfy existing game code that cancels a pending state change.
    }

    void GameStateManager::ReloadState()
    {
        if (mGameStateStack.empty())
            return;

        auto* state = mGameStateStack.back().get();
        state->Unload();
        state->Load();
    }

}
