#version 300 es

/**
 * \file
 * \author Sehoon Kim
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#ifdef GL_ES
precision mediump float;
precision mediump int;
#endif

layout(location = 0) in vec2 aPos;

uniform mat3 uToNDC;
uniform mat3 uModel;
uniform vec2 uSDFScale;

out vec2 vTestPoint;

void main()
{
    vec3 world = uModel * vec3(aPos, 1.0);

    vTestPoint = aPos * uSDFScale;

    vec3 ndc = uToNDC * world;
    gl_Position = vec4(ndc.xy, 0.0, 1.0);
}
