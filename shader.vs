#version 330

layout (location = 0) in vec3 Position;

layout (location = 1) in vec3 _Color;

uniform mat4 gWorld;
out vec4 _Col;

void main()
{
    gl_Position = gWorld * vec4(Position, 1.0);
    _Col = vec4(clamp(_Color, 0.1, 1.0), 1.0);
}