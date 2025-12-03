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
#include "Texture.hpp"

#include "CS200/Image.hpp"
#include "CS200/IRenderer2D.hpp"

#include "OpenGL/GL.hpp"
#include "OpenGL/Texture.hpp"

namespace CS230
{
    Texture::Texture(const std::filesystem::path& file_name)
    {
        CS200::Image img(file_name, false);
        size = img.GetSize();
        sourcePath = file_name;

        textureHandle = OpenGL::CreateTextureFromImage(img);
    }

    Texture::Texture(OpenGL::TextureHandle handle, Math::ivec2 texture_size) : textureHandle(handle), size(texture_size)
    {

    }

    Texture::Texture(Texture&& other) noexcept : textureHandle(other.textureHandle), size(other.size), sourcePath(std::move(other.sourcePath))
    {
        other.textureHandle = 0;
        other.size = {0, 0};
    }

    Texture& Texture::operator=(Texture&& other) noexcept
    {
        if(this != &other)
        {
            std::swap(textureHandle, other.textureHandle);
            std::swap(size, other.size);
            std::swap(sourcePath, other.sourcePath);
        }
        return *this;
    }

    Texture::~Texture()
    {
        if(textureHandle)
        {
            GL::DeleteTextures(1, &textureHandle);
            textureHandle = 0;
        }
    }

    void Texture::Draw(const Math::TransformationMatrix& display_matrix, unsigned int color)
    {
        if(textureHandle == 0 || size.x <= 0 || size.y <= 0)
        { return; }
        Draw(display_matrix, {0,0}, GetSize(), static_cast<CS200::RGBA>(color));
    }

    void Texture::Draw(const Math::TransformationMatrix& display_matrix, Math::ivec2 texel_position, Math::ivec2 frame_size, unsigned int color /* = 0xFFFFFFFF */)
    {
        if(textureHandle == 0 || size.x <= 0 || size.y <= 0)
        { return; }
        auto scale = Math::ScaleMatrix(static_cast<Math::vec2>(frame_size));
        auto recenter = Math::TranslationMatrix(static_cast<Math::vec2>(frame_size) / 2.0);

        const double u_left = static_cast<double>(texel_position.x) / size.x;
        const double u_right = static_cast<double>(texel_position.x + frame_size.x) / size.x;
        const double v_top = static_cast<double>(texel_position.y) / size.y;
        const double v_bottom = static_cast<double>(texel_position.y + frame_size.y) / size.y;

        const Math::vec2 texture_coord_bl = { u_left, v_bottom };
        const Math::vec2 texture_coord_tr = { u_right, v_top };

        auto& r2d = Engine::GetRenderer2D();
        r2d.DrawQuad(display_matrix *recenter *scale , textureHandle, texture_coord_bl, texture_coord_tr, static_cast<CS200::RGBA>(color));
    } 

    Math::ivec2 Texture::GetSize() const
    {
        return size;
    }
}
