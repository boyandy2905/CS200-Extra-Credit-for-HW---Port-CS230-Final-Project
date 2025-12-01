/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Sehoon Kim
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "TextureManager.hpp"
#include "CS200/IRenderer2D.hpp"
#include "CS200/NDC.hpp"
#include "Engine.hpp"
#include "Logger.hpp"
#include "OpenGL/Framebuffer.hpp"
#include "OpenGL/GL.hpp"
#include "Texture.hpp"
#include <algorithm>
#include <memory>

std::shared_ptr<CS230::Texture> CS230::TextureManager::Load(const std::filesystem::path& file_name)
{
    if(auto it = textures.find(file_name); it != textures.end())
    {
        return it->second;
    }

    auto tex = std::shared_ptr<CS230::Texture>(new CS230::Texture(file_name));
    textures.emplace(file_name, tex);
    return tex;
}

void CS230::TextureManager::Unload()
{
    textures.clear();
}

namespace
{
    struct RenderToTextureState {
        GLint   old_viewport[4] {0,0,0,0};
        GLfloat old_clear_color[4] {0.f,0.f,0.f,0.f};

        OpenGL::FramebufferWithColor framebuffer{};
        Math::ivec2 size{0,0};
        bool active = false;
    };

    RenderToTextureState g_rtt_state;
}

void CS230::TextureManager::StartRenderTextureMode(int width, int height)
{
    if(width <= 0 || height <= 0)
    {
        return;
    }

    GL::GetIntegerv(GL_VIEWPORT, g_rtt_state.old_viewport);
    GL::GetFloatv(GL_COLOR_CLEAR_VALUE, g_rtt_state.old_clear_color);

    Engine::GetRenderer2D().EndScene();

    g_rtt_state.size = { width, height };
    g_rtt_state.framebuffer = OpenGL::CreateFramebufferWithColor(g_rtt_state.size);
    GL::BindFramebuffer(GL_FRAMEBUFFER, g_rtt_state.framebuffer.Framebuffer);
    GL::Viewport(0, 0, width, height);

    GL::ClearColor(0.f, 0.f, 0.f, 0.f);
    GL::Clear(GL_COLOR_BUFFER_BIT);

    const double sx = 2.0 / static_cast<double>(width);
    const double sy = -2.0 / static_cast<double>(height);
    Math::TransformationMatrix ndc = Math::TranslationMatrix(Math::vec2{-1.0, 1.0}) * Math::ScaleMatrix(Math::vec2{sx, sy});
    Engine::GetRenderer2D().BeginScene(ndc);

    g_rtt_state.active = true;
}

std::shared_ptr<CS230::Texture> CS230::TextureManager::EndRenderTextureMode()
{
    if(!g_rtt_state.active)
    {
        return nullptr;
    }

    Engine::GetRenderer2D().EndScene();

    GL::BindFramebuffer(GL_FRAMEBUFFER, 0);
    GL::Viewport(g_rtt_state.old_viewport[0], g_rtt_state.old_viewport[1], g_rtt_state.old_viewport[2], g_rtt_state.old_viewport[3]);
    GL::ClearColor(g_rtt_state.old_clear_color[0], g_rtt_state.old_clear_color[1], g_rtt_state.old_clear_color[2], g_rtt_state.old_clear_color[3]);

    OpenGL::TextureHandle color_tex = g_rtt_state.framebuffer.ColorAttachment;
    auto result = std::shared_ptr<CS230::Texture>(new CS230::Texture(color_tex, g_rtt_state.size));

    if(g_rtt_state.framebuffer.Framebuffer != 0)
    {
        auto fb = g_rtt_state.framebuffer.Framebuffer;
        GL::DeleteFramebuffers(1, &fb);
    }
    g_rtt_state.framebuffer.Framebuffer = 0;
    g_rtt_state.framebuffer.ColorAttachment = 0;

    const Math::ivec2 screen_size{ g_rtt_state.old_viewport[2], g_rtt_state.old_viewport[3] };
    Engine::GetRenderer2D().BeginScene(CS200::build_ndc_matrix(screen_size));

    g_rtt_state.active = false;
    return result;
}
