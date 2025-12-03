/**
 * \file
 * \author Rudy Castan
 * \author Sehoon Kim
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "ImmediateRenderer2D.hpp"
#include "Renderer2DUtils.hpp"
#include "NDC.hpp"
#include "RGBA.hpp"

#include "Engine/Matrix.hpp"
#include "Engine/Texture.hpp"

#include "OpenGL/Buffer.hpp"
#include "OpenGL/GL.hpp"

#include <array>
#include <cmath>
#include <utility>

namespace CS200
{
    namespace
    {
        constexpr float QUADVERTS[] = {
            -0.5f, -0.5f, 0.0f, 0.0f,
             0.5f, -0.5f, 1.0f, 0.0f,
            -0.5f,  0.5f, 0.0f, 1.0f,
             0.5f,  0.5f, 1.0f, 1.0f,
        };
        constexpr unsigned short QUAD_INDICES[] = { 0, 1, 2, 2, 1, 3 };

        constexpr float SDFPOS[] = {
            -0.5f, -0.5f,
             0.5f, -0.5f,
            -0.5f,  0.5f,
             0.5f,  0.5f,
        };
    }

    ImmediateRenderer2D::ImmediateRenderer2D(ImmediateRenderer2D&& other) noexcept
    {
        *this = std::move(other);
    }

    ImmediateRenderer2D& ImmediateRenderer2D::operator=(ImmediateRenderer2D&& other) noexcept
    {
        if(this != &other)
        {
            std::swap(quadVAO, other.quadVAO);
            std::swap(quadVBO, other.quadVBO);
            std::swap(quadIBO, other.quadIBO);
            std::swap(quadShader, other.quadShader);
            std::swap(cameraUBO, other.cameraUBO);

            std::swap(sdfVAO, other.sdfVAO);
            std::swap(sdfVBO, other.sdfVBO);
            std::swap(sdfShader, other.sdfShader);

            std::swap(viewProjection, other.viewProjection);
        }
        return *this;
    }

    ImmediateRenderer2D::~ImmediateRenderer2D()
    {
        Shutdown();
    }

    void ImmediateRenderer2D::Init()
    {
        GL::GenVertexArrays(1, &quadVAO);
        GL::BindVertexArray(quadVAO);
        GL::GenBuffers(1, &quadVBO);
        GL::BindBuffer(GL_ARRAY_BUFFER, quadVBO);
        GL::BufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(QUADVERTS)), QUADVERTS, GL_STATIC_DRAW);
        GL::GenBuffers(1, &quadIBO);
        GL::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIBO);
        GL::BufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(QUAD_INDICES)), QUAD_INDICES, GL_STATIC_DRAW);
        GL::EnableVertexAttribArray(0);
        GL::VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, reinterpret_cast<const void*>(0));
        GL::EnableVertexAttribArray(1);
        GL::VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, reinterpret_cast<const void*>(sizeof(float)*2));
        GL::BindVertexArray(0);

        GL::GenVertexArrays(1, &sdfVAO);
        GL::BindVertexArray(sdfVAO);
        GL::GenBuffers(1, &sdfVBO);
        GL::BindBuffer(GL_ARRAY_BUFFER, sdfVBO);
        GL::BufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(SDFPOS)), SDFPOS, GL_STATIC_DRAW);
        GL::EnableVertexAttribArray(0);
        GL::VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, reinterpret_cast<const void*>(0));
        GL::BindVertexArray(0);

        using std::filesystem::path;
        quadShader = OpenGL::CreateShader(path{"Assets/shaders/ImmediateRenderer2D/quad.vert"}, path{"Assets/shaders/ImmediateRenderer2D/quad.frag"});
        sdfShader  = OpenGL::CreateShader(path{"Assets/shaders/ImmediateRenderer2D/sdf.vert"}, path{"Assets/shaders/ImmediateRenderer2D/sdf.frag"});
    }

    void ImmediateRenderer2D::Shutdown()
    {
        if(quadIBO) { GL::DeleteBuffers(1, &quadIBO); quadIBO = 0; }
        if(quadVBO) { GL::DeleteBuffers(1, &quadVBO); quadVBO = 0; }
        if(quadVAO) { GL::DeleteVertexArrays(1, &quadVAO); quadVAO = 0; }
        if(sdfVBO)  { GL::DeleteBuffers(1, &sdfVBO);  sdfVBO  = 0; }
        if(sdfVAO)  { GL::DeleteVertexArrays(1, &sdfVAO); sdfVAO = 0; }

        if(quadShader.Shader) { OpenGL::DestroyShader(quadShader); }
        if(sdfShader.Shader)  { OpenGL::DestroyShader(sdfShader); }

        if(cameraUBO) { GL::DeleteBuffers(1, &cameraUBO); cameraUBO = 0; }
    }

    void ImmediateRenderer2D::BeginScene(const Math::TransformationMatrix& view_projection)
    {
        viewProjection = view_projection;
    }

    void ImmediateRenderer2D::EndScene()
    {
    }

    void ImmediateRenderer2D::DrawQuad(const Math::TransformationMatrix& transform, OpenGL::TextureHandle texture, Math::vec2 texture_coord_bl, Math::vec2 texture_coord_tr, CS200::RGBA tintColor)
    {
        GL::UseProgram(quadShader.Shader);

        const auto vpMat = Renderer2DUtils::to_opengl_mat3(viewProjection);
        const auto model = Renderer2DUtils::to_opengl_mat3(transform);

        auto loc = quadShader.UniformLocations.find("u_ViewProj");
        if(loc != quadShader.UniformLocations.end())
        {
            GL::UniformMatrix3fv(loc->second, 1, GL_FALSE, vpMat.data());
        }

        loc = quadShader.UniformLocations.find("u_Model");
        if(loc != quadShader.UniformLocations.end())
        {
            GL::UniformMatrix3fv(loc->second, 1, GL_FALSE, model.data());
        }

        loc = quadShader.UniformLocations.find("u_TexCoordBL");
        if(loc != quadShader.UniformLocations.end())
        {
            GL::Uniform2f(loc->second, static_cast<GLfloat>(texture_coord_bl.x), static_cast<GLfloat>(texture_coord_bl.y));
        }

        loc = quadShader.UniformLocations.find("u_TexCoordTR");
        if(loc != quadShader.UniformLocations.end())
        {
            GL::Uniform2f(loc->second, static_cast<GLfloat>(texture_coord_tr.x), static_cast<GLfloat>(texture_coord_tr.y));
        }

        const auto tint = CS200::unpack_color(tintColor);
        loc = quadShader.UniformLocations.find("u_Tint");
        if(loc != quadShader.UniformLocations.end())
        {
            GL::Uniform4f(loc->second, tint[0], tint[1], tint[2], tint[3]);
        }

        GL::ActiveTexture(GL_TEXTURE0);
        GL::BindTexture(GL_TEXTURE_2D, texture);

        GL::BindVertexArray(quadVAO);
        GL::DrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, reinterpret_cast<const void*>(0));
        GL::BindVertexArray(0);
    }

    void ImmediateRenderer2D::DrawSDF(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width, SDFShape sdf_shape)
    {
        const auto sdf = Renderer2DUtils::CalculateSDFTransform(transform, line_width);
        GL::UseProgram(sdfShader.Shader);

        GLint vp[4] = {0,0,1,1};
        GL::GetIntegerv(GL_VIEWPORT, vp);
        const Math::ivec2 screen_size{ vp[2], vp[3] };
        const Math::TransformationMatrix ndc = CS200::build_ndc_matrix(screen_size);
        const auto toNDC = Renderer2DUtils::to_opengl_mat3(ndc);

        auto loc = sdfShader.UniformLocations.find("uToNDC");
        if(loc != sdfShader.UniformLocations.end())
        {
            GL::UniformMatrix3fv(loc->second, 1, GL_FALSE, toNDC.data());
        }

        loc = sdfShader.UniformLocations.find("uModel");
        if(loc != sdfShader.UniformLocations.end())
        {
            GL::UniformMatrix3fv(loc->second, 1, GL_FALSE, sdf.QuadTransform.data());
        }

        loc = sdfShader.UniformLocations.find("uSDFScale");
        if(loc != sdfShader.UniformLocations.end())
        {
            GL::Uniform2f(loc->second, sdf.QuadSize[0], sdf.QuadSize[1]);
        }

        const auto fc = CS200::unpack_color(fill_color);
        const auto lc = CS200::unpack_color(line_color);

        loc = sdfShader.UniformLocations.find("uFillColor");
        if(loc != sdfShader.UniformLocations.end())
        {
            GL::Uniform4f(loc->second, fc[0], fc[1], fc[2], fc[3]);
        }

        loc = sdfShader.UniformLocations.find("uLineColor");
        if(loc != sdfShader.UniformLocations.end())
        {
            GL::Uniform4f(loc->second, lc[0], lc[1], lc[2], lc[3]);
        }

        loc = sdfShader.UniformLocations.find("uWorldSize");
        if(loc != sdfShader.UniformLocations.end())
        {
            GL::Uniform2f(loc->second, sdf.WorldSize[0], sdf.WorldSize[1]);
        }

        loc = sdfShader.UniformLocations.find("uLineWidth");
        if(loc != sdfShader.UniformLocations.end())
        {
            GL::Uniform1f(loc->second, static_cast<float>(line_width));
        }

        loc = sdfShader.UniformLocations.find("uShape");
        if(loc != sdfShader.UniformLocations.end())
        {
            const GLint shape = (sdf_shape == SDFShape::Rectangle) ? 1 : 0;
            GL::Uniform1i(loc->second, shape);
        }

        GL::BindVertexArray(sdfVAO);
        GL::DrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        GL::BindVertexArray(0);
    }

    /*------------------------------Upper----------New------------------------------*/

    void ImmediateRenderer2D::DrawCircle(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width)
    {
        DrawSDF(transform, fill_color, line_color, line_width, SDFShape::Circle);
    }

    void ImmediateRenderer2D::DrawRectangle(const Math::TransformationMatrix& transform, CS200::RGBA fill_color, CS200::RGBA line_color, double line_width)
    {
        DrawSDF(transform, fill_color, line_color, line_width, SDFShape::Rectangle);
    }

    void ImmediateRenderer2D::DrawLine(const Math::TransformationMatrix& transform, Math::vec2 start_point, Math::vec2 end_point, CS200::RGBA line_color, double line_width)
    {
        const auto line_transform = Renderer2DUtils::CalculateLineTransform(transform, start_point, end_point, line_width);
        DrawSDF(line_transform, line_color, line_color, line_width, SDFShape::Rectangle);
    }

    void ImmediateRenderer2D::DrawLine(Math::vec2 start_point, Math::vec2 end_point, CS200::RGBA line_color, double line_width)
    {
        DrawLine(Math::TransformationMatrix{}, start_point, end_point, line_color, line_width);
    }
}
