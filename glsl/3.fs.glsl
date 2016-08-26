#version 330 core

in vec4 pos;
out vec4 color;

void main(void){
    color = vec4(noise3(pos.xyz * 3.0) + 0.5, 1.0);
}
