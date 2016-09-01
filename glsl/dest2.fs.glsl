#version 330 core

in vec4 pos;
out vec4 color;

uniform vec2 dims;
uniform float width;

uniform sampler2D shampler;

void main() {
    vec2 tc = pos.st;
    tc *= mat2(0.5, 0.0, 0.0, 0.5);
    tc += vec2(0.5);

    vec2 offs = vec2(1. / dims.x, 1. / dims.y);

    vec2 src = tc;

    float width = 3.2;
    vec2 tc4 = src;
    vec2 tc1 = src + vec2(0.0, -offs.t * width);
    vec2 tc3 = src + vec2(-offs.s * width, 0.0);
    vec2 tc5 = src + vec2(offs.s * width, 0.0);
    vec2 tc7 = src + vec2(0.0, offs.t * width);

    vec2 tc0 = src + vec2(-offs.s * width, -offs.t * width);
    vec2 tc2 = src + vec2(offs.s * width, -offs.t * width);
    vec2 tc6 = src + vec2(-offs.s * width, offs.t * width);
    vec2 tc8 = src + vec2(offs.s * width, offs.t * width);

    vec4 col0 = texture(shampler, tc0);
    vec4 col1 = texture(shampler, tc1);
    vec4 col2 = texture(shampler, tc2);
    vec4 col3 = texture(shampler, tc3);
    vec4 col4 = texture(shampler, tc4);
    vec4 col5 = texture(shampler, tc5);
    vec4 col6 = texture(shampler, tc6);
    vec4 col7 = texture(shampler, tc7);
    vec4 col8 = texture(shampler, tc8);

    // pass transformed pixel out with no convolution
    color = col4;

//    color = col4 + col1 + col3 + col5 + col7 * 0.1;
//    color = col4 * 4.1 - (col1 + col3 + col5 + col7);
}
