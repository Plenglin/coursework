#version 330 core

uniform vec3 draw_color;
out vec4 color;

void main()
{

color.rgb = draw_color;
color.a=1;


}
