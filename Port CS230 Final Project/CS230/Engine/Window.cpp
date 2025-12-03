/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Sehoon Kim
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "Engine.hpp"
#include "Error.hpp"
#include "Logger.hpp"
#include "Window.hpp"

#include "CS200/RenderingAPI.hpp"

#include <OpenGL/GL.hpp>

#include <functional>
#include <GL/glew.h>
#include <SDL.h>
#include <sstream>

namespace
{
    void hint_gl(SDL_GLattr attr, int value)
    {
        if (const auto success = SDL_GL_SetAttribute(attr, value); success != 0)
        {
            Engine::GetLogger().LogError(std::string{ "Failed to Set GL Attribute: " } + SDL_GetError());
        }
    }
}

namespace CS230
{
    void Window::Start(std::string_view title)
    {
        setupSDLWindow(title);
        if (!m_headless)
        {
            setupOpenGL();

            // Get actual drawable size for high-DPI displays
            SDL_GL_GetDrawableSize(sdlWindow, &size.x, &size.y);

            // Set initial clear color through our rendering abstraction
            CS200::RenderingAPI::SetClearColor(default_background);
        }
        else
        {
            Engine::GetLogger().LogEvent("Running in headless mode (no window / GL context).");
            size = { default_width, default_height };
        }
    }

    void Window::Update()
    {
        if (!m_headless)
        {
            SDL_GL_SwapWindow(sdlWindow);
            int current_width, current_height;
            SDL_GetWindowSize(sdlWindow, &current_width, &current_height);

            ForceResize(current_width, current_height);

            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                    m_is_closed = true;

                if (eventCallback)
                    eventCallback(event);
            }
        }
        else
        {
            // In headless mode we don't have a window to swap. Poll events to allow SDL to run internal loops if possible.
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                    m_is_closed = true;
                if (eventCallback)
                    eventCallback(event);
            }
        }
    }

    bool Window::IsClosed() const
    {
        return m_is_closed;
    }

    Math::ivec2 Window::GetSize() const
    {
        if (!m_headless)
        {
            int width, height;
            SDL_GetWindowSize(sdlWindow, &width, &height);
            return Math::ivec2{ width, height };
        }
        return size;
    }

    void Window::Clear(CS200::RGBA color)
    {
        const float red   = static_cast<float>((color >> 24) & 0xFF) / 255.0f;
        const float green = static_cast<float>((color >> 16) & 0xFF) / 255.0f;
        const float blue  = static_cast<float>((color >> 8)  & 0xFF) / 255.0f;
        const float alpha = static_cast<float>(color & 0xFF) / 255.0f;

    GL::ClearColor(red, green, blue, alpha);
    GL::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Window::ForceResize(int current_width, int current_height)
    {
        if (current_width != size.x || current_height != size.y)
        {
            Engine::GetLogger().LogEvent("Window Resized");
            SDL_SetWindowSize(sdlWindow, current_width, current_height);
            size = { current_width, current_height };
        }
    }

    void Window::SetEventCallback(WindowEventCallback callback)
    {
        eventCallback = callback;
    }

    void Window::setupSDLWindow(std::string_view title)
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            throw_error_message("Failed to init SDL: ", SDL_GetError());
        }

#if defined(IS_WEBGL2)
        hint_gl(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        hint_gl(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        hint_gl(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
        hint_gl(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif
        hint_gl(SDL_GL_DOUBLEBUFFER, true);
        hint_gl(SDL_GL_STENCIL_SIZE, 8);
        hint_gl(SDL_GL_DEPTH_SIZE, 24);
        hint_gl(SDL_GL_RED_SIZE, 8);
        hint_gl(SDL_GL_GREEN_SIZE, 8);
        hint_gl(SDL_GL_BLUE_SIZE, 8);
        hint_gl(SDL_GL_ALPHA_SIZE, 8);
        hint_gl(SDL_GL_MULTISAMPLEBUFFERS, 1);
        hint_gl(SDL_GL_MULTISAMPLESAMPLES, 4);

        sdlWindow = SDL_CreateWindow(title.data(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                     default_width, default_height,
                                     SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
        if (!sdlWindow)
        {
            Engine::GetLogger().LogEvent(std::string("Failed to create window: ") + SDL_GetError());
            m_headless = true;
            return;
        }
    }

    void Window::setupOpenGL()
    {
        glContext = SDL_GL_CreateContext(sdlWindow);
        if (!glContext)
        {
            throw_error_message("Failed to create OpenGL context: ", SDL_GetError());
        }

        SDL_GL_MakeCurrent(sdlWindow, glContext);

        if (glewInit() != GLEW_OK)
        {
            throw_error_message("Unable to initialize GLEW", "");
        }

        constexpr int ADAPTIVE_VSYNC = -1;
        constexpr int VSYNC = 1;
        if (SDL_GL_SetSwapInterval(ADAPTIVE_VSYNC) != 0)
            SDL_GL_SetSwapInterval(VSYNC);

        CS200::RenderingAPI::Init();
    }

    Window::~Window()
    {
        if (glContext) SDL_GL_DeleteContext(glContext);
        if (sdlWindow) SDL_DestroyWindow(sdlWindow);
        SDL_Quit();
    }

    SDL_Window* Window::GetSDLWindow() const { return sdlWindow; }
    SDL_GLContext Window::GetGLContext() const { return glContext; }
}
