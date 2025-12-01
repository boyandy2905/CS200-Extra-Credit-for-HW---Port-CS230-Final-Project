/**
 * \file
 * \author Rudy Castan
 * \author Sehoon Kim
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "Environment.hpp"
#include "GL.hpp"
#include "Texture.hpp"
#include "CS200/Image.hpp"

namespace OpenGL
{
    [[nodiscard]] TextureHandle
    CreateTextureFromImage(const CS200::Image& image, Filtering filtering, Wrapping wrapping) noexcept
    {
        const Math::ivec2 size = image.GetSize();
        const std::span<const CS200::RGBA> pixels
        {
            image.data(),
            static_cast<size_t>(size.x) * static_cast<size_t>(size.y)
        };

        return CreateTextureFromMemory(size, pixels, filtering, wrapping);
    }

    [[nodiscard]] TextureHandle
    CreateTextureFromMemory(Math::ivec2 size, std::span<const CS200::RGBA> colors, Filtering filtering, Wrapping wrapping) noexcept
    {
        TextureHandle handle = 0u;
        GL::GenTextures(1, &handle);
        GL::BindTexture(GL_TEXTURE_2D, handle);

        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filtering));
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(filtering));

        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping));
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapping));

        const void* data_ptr = colors.empty() ? nullptr : static_cast<const void*>(colors.data());
        GL::TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data_ptr);

        GL::BindTexture(GL_TEXTURE_2D, 0);

        return handle;
    }

    [[nodiscard]] TextureHandle
    CreateRGBATexture(Math::ivec2 size, Filtering filtering, Wrapping  wrapping) noexcept
    {
        TextureHandle handle = 0u;
        GL::GenTextures(1, &handle);
        GL::BindTexture(GL_TEXTURE_2D, handle);

        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filtering));
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(filtering));

        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping));
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapping));

        GL::TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        GL::BindTexture(GL_TEXTURE_2D, 0);

        return handle;
    }

    void SetFiltering(TextureHandle texture_handle, Filtering filtering) noexcept
    {
        if(texture_handle == 0u)
        {
            return;
        }

        GL::BindTexture(GL_TEXTURE_2D, texture_handle);
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filtering));
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(filtering));
        GL::BindTexture(GL_TEXTURE_2D, 0);
    }

    void SetWrapping(TextureHandle texture_handle, Wrapping wrapping, TextureCoordinate coord) noexcept
    {
        if(texture_handle == 0u)
        {
            return;
        }

        GL::BindTexture(GL_TEXTURE_2D, texture_handle);
        const GLint wrap = static_cast<GLint>(wrapping);

        if(coord == TextureCoordinate::S || coord == TextureCoordinate::Both)
        { GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap); }
        if(coord == TextureCoordinate::T || coord == TextureCoordinate::Both)
        { GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap); }

        GL::BindTexture(GL_TEXTURE_2D, 0);
    }
}
