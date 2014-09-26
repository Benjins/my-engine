#version 330

layout (location = 0) in vec3 Position;

layout (location = 2) in vec2 UV;

out vec2 uv_coord;

void main()
{
    gl_Position = vec4(Position, 1.0);
	uv_coord = UV;
}