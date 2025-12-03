#version 300 es

/**
 * \file
 * \author Sehoon Kim
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoord;

uniform mat3 u_ViewProj;
uniform mat3 u_Model;
uniform vec2 u_TexCoordBL;
uniform vec2 u_TexCoordTR;

out vec2 vUV;

void main()
{
    vUV = mix(u_TexCoordBL, u_TexCoordTR, a_TexCoord);
    vec3 p = u_ViewProj * (u_Model * vec3(a_Position, 1.0));
    gl_Position = vec4(p.xy, 0.0, 1.0);
}
