#version 130

out vec4 FragColor;

uniform vec4 _color;

void main()
{
    FragColor = vec4(_color.xy,1.0,1.0);
}