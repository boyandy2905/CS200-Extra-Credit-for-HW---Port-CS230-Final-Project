/**
 * \file
 * \author Rudy Castan
 * \author Sehoon Kim
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "RenderingAPI.hpp"
#include "OpenGL/GL.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Error.hpp"
#include "Engine/Logger.hpp"
#include "OpenGL/Environment.hpp"
#include <GL/glew.h>
#include <cassert>

namespace
{
#if defined(DEVELOPER_VERSION) && not defined(IS_WEBGL2)
    void OpenGLMessageCallback(
        [[maybe_unused]] unsigned source, [[maybe_unused]] unsigned type, [[maybe_unused]] unsigned id, unsigned severity, [[maybe_unused]] int length, const char* message,
        [[maybe_unused]] const void* userParam)
    {
        switch (severity)
        {
            case GL_DEBUG_SEVERITY_HIGH: Engine::GetLogger().LogError(message); return;
            case GL_DEBUG_SEVERITY_MEDIUM: Engine::GetLogger().LogError(message); return;
            case GL_DEBUG_SEVERITY_LOW: Engine::GetLogger().LogVerbose(message); return;
            case GL_DEBUG_SEVERITY_NOTIFICATION: Engine::GetLogger().LogVerbose(message); return;
        }

        assert(false && "Unknown severity level!");
    }
#endif
}

namespace CS200::RenderingAPI
{
    void Init() noexcept
    {
        GLint major = 0, minor = 0;
        GL::GetIntegerv(GL_MAJOR_VERSION, &major);
        GL::GetIntegerv(GL_MINOR_VERSION, &minor);
        if (OpenGL::version(major, minor) < OpenGL::version(OpenGL::MinimumRequiredMajorVersion, OpenGL::MinimumRequiredMinorVersion))
            throw_error_message("Unsupported OpenGL version ", major, '.', minor, "\n We need OpenGL ", OpenGL::MinimumRequiredMajorVersion, '.', OpenGL::MinimumRequiredMinorVersion, " or higher");

        if (OpenGL::MajorVersion == 0)
        {
            OpenGL::MajorVersion = major;
            OpenGL::MinorVersion = minor;
        }

        GL::GetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &OpenGL::MaxTextureImageUnits);
        GL::GetIntegerv(GL_MAX_TEXTURE_SIZE, &OpenGL::MaxTextureSize);

#if defined(DEVELOPER_VERSION) && not defined(IS_WEBGL2)
        // Debug callback functionality requires OpenGL 4.3+ or KHR_debug extension
        if (OpenGL::current_version() >= OpenGL::version(4, 3))
        {
            GL::Enable(GL_DEBUG_OUTPUT);
            GL::Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            GL::DebugMessageCallback(OpenGLMessageCallback, nullptr);
            GL::DebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
        }
#endif

        GL::Enable(GL_BLEND);
        GL::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        GL::Disable(GL_DEPTH_TEST);

        Engine::GetLogger().LogDebug(std::string("GL_VENDOR:   ") + reinterpret_cast<const char*>(GL::GetString(GL_VENDOR)));
        Engine::GetLogger().LogDebug(std::string("GL_RENDERER: ") + reinterpret_cast<const char*>(GL::GetString(GL_RENDERER)));
        Engine::GetLogger().LogDebug(std::string("GL_VERSION:  ") + reinterpret_cast<const char*>(GL::GetString(GL_VERSION)));
        Engine::GetLogger().LogDebug(std::string("GL_SHADING_LANGUAGE_VERSION: ") + reinterpret_cast<const char*>(GL::GetString(GL_SHADING_LANGUAGE_VERSION)));

        major = 0, minor = 0;
        GL::GetIntegerv(GL_MAJOR_VERSION, &major);
        GL::GetIntegerv(GL_MINOR_VERSION, &minor);
        Engine::GetLogger().LogDebug("GL_MAJOR_VERSION: " + std::to_string(major) + " GL_MINOR_VERSION: " + std::to_string(minor));

        GLint max_vertices = 0, max_indices = 0, max_units = 0, max_size = 0;
        GLint viewport[2] = {0, 0};
        GL::GetIntegerv(GL_MAX_ELEMENTS_VERTICES, &max_vertices);
        GL::GetIntegerv(GL_MAX_ELEMENTS_INDICES, &max_indices);
        GL::GetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_units);
        GL::GetIntegerv(GL_MAX_TEXTURE_SIZE, &max_size);
        GL::GetIntegerv(GL_MAX_VIEWPORT_DIMS, viewport);

        Engine::GetLogger().LogDebug("GL_MAX_ELEMENTS_VERTICES: " + std::to_string(max_vertices));
        Engine::GetLogger().LogDebug("GL_MAX_ELEMENTS_INDICES:  " + std::to_string(max_indices));
        Engine::GetLogger().LogDebug("GL_MAX_TEXTURE_IMAGE_UNITS: " + std::to_string(max_units));
        Engine::GetLogger().LogDebug("GL_MAX_TEXTURE_SIZE: " + std::to_string(max_size));
        Engine::GetLogger().LogDebug("GL_MAX_VIEWPORT_DIMS: " + std::to_string(viewport[0]) + " x " + std::to_string(viewport[1]));
    }

    void SetClearColor(CS200::RGBA color) noexcept
    {
        const auto rgba = CS200::unpack_color(color);
        GL::ClearColor(rgba[0], rgba[1], rgba[2], rgba[3]);
    }

    void Clear() noexcept
    {
        GL::Clear(GL_COLOR_BUFFER_BIT);
    }

    void SetViewport(Math::ivec2 size, Math::ivec2 anchor_left_bottom) noexcept
    {
        GL::Viewport(anchor_left_bottom.x, anchor_left_bottom.y, size.x, size.y);
    }
}
