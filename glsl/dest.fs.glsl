#version 330 core

in vec4 pos;
out vec4 color;

uniform vec2 dims;
uniform float width;
//uniform float amp;
uniform float scaleCoef;

uniform sampler2D shampler;

vec3 rgb2hsv(vec3 c) {
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

void main() {
    vec2 tc = pos.st;
    tc *= mat2(0.5, 0.0, 0.0, 0.5);
    tc += vec2(0.5);

    vec3 pixel = texture(shampler, tc).rgb;

    float s = rgb2hsv(pixel).s;

    mat2 sca = mat2(1., 0., 0., 1.);
    vec2 offs = vec2(1. / dims.x, 1. / dims.y);

    vec2 src = tc * sca;

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
//    color = col4;

//    color = col4 + col1 + col3 + col5 + col7 * 0.196;
    color = col4 * 5.0 - (col1 + col3 + col5 + col7);
}
