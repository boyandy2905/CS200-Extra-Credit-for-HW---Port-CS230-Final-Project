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

// 정점 셰이더에서 전달받는 입력 변수 (vLocal -> vTestPoint 이름 변경)
in vec2 vTestPoint; 

out vec4 FragColor;

uniform vec4  uFillColor;
uniform vec4  uLineColor;
uniform vec2  uWorldSize;
uniform float uLineWidth;
uniform int   uShape;

float sdCircle(vec2 p, float r) 
{
    return length(p) - r;
}

float sdRect(vec2 p, vec2 halfSize) 
{
    vec2 q = abs(p) - halfSize;
    return length(max(q, 0.0)) + min(max(q.x, q.y), 0.0);
}

vec4 evaluate_color(float sd)
{
    float fill_alpha = (sd < 0.0) ? 1.0 : 0.0;
    vec4 fill_color = vec4(uFillColor.rgb, fill_alpha * uFillColor.a);

    float outline_alpha = (abs(sd) < 0.5 * uLineWidth) ? 1.0 : 0.0;
    vec4 line_color = vec4(uLineColor.rgb, outline_alpha * uLineColor.a);

    if (line_color.a > 0.0)
        return line_color;
    return fill_color;
}

void main()
{
    float sd = 0.0;
    if (uShape == 0)
    {
        float radius = min(uWorldSize.x, uWorldSize.y) * 0.5;
        sd = sdCircle(vTestPoint, radius);
    }
    else
    {
        sd = sdRect(vTestPoint, 0.5 * uWorldSize);
    }

    vec4 color = evaluate_color(sd);

    if (color.a <= 0.0)
        discard;

    FragColor = color;
}
