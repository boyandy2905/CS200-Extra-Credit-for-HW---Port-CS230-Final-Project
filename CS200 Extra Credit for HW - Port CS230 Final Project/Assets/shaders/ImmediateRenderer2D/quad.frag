#version 300 es
precision         mediump float;
precision mediump sampler2D;

/**
 * \file
 * \author Sehoon Kim
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

out vec4 FragColor;

in vec2 vUV;

uniform sampler2D u_Texture;
uniform vec4      u_Tint;

void main()
{
    vec4 tex = texture(u_Texture, vUV);
    FragColor = tex * u_Tint;
}
