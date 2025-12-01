/**
 * \file
 * \author Rudy Castan
 * \author Sehoon Kim
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "Buffer.hpp"

#include "GL.hpp"

namespace OpenGL
{
    BufferHandle CreateBuffer(BufferType type, GLsizeiptr size_in_bytes) noexcept
    {
        BufferHandle buffer{};

        GL::GenBuffers(1, &buffer);
        GL::BindBuffer(static_cast<GLenum>(type), buffer);
        GL::BufferData(static_cast<GLenum>(type), size_in_bytes, nullptr, GL_DYNAMIC_DRAW);
        GL::BindBuffer(static_cast<GLenum>(type), 0);

        return buffer;
    }

    BufferHandle CreateBuffer(BufferType type, std::span<const std::byte> static_buffer_data) noexcept
    {
        BufferHandle buffer{};
        
        GL::GenBuffers(1, &buffer);
        GL::BindBuffer(static_cast<GLenum>(type), buffer);
        GL::BufferData(static_cast<GLenum>(type), static_cast<GLsizeiptr>(static_buffer_data.size()), static_buffer_data.data(), GL_STATIC_DRAW);
        GL::BindBuffer(static_cast<GLenum>(type), 0);

        return buffer;
    }

    void UpdateBufferData(BufferType type, BufferHandle buffer, std::span<const std::byte> data_to_copy, GLsizei starting_offset) noexcept
    {
        GL::BindBuffer(static_cast<GLenum>(type), buffer);
        GL::BufferSubData(static_cast<GLenum>(type), starting_offset, static_cast<GLsizeiptr>(data_to_copy.size()), data_to_copy.data());
        GL::BindBuffer(static_cast<GLenum>(type), 0);
    }
}
