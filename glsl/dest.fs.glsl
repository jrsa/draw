#version 330 core

in vec4 pos;
out vec4 color;

uniform sampler2D shampler;

void main(void){
    vec2 tc = pos.st;
    tc *= mat2(0.5, 0.0, 0.0, 0.5);
    tc += vec2(0.5);
    color = texture(shampler, tc);
}
