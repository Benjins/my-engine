#version 330

layout (location = 0) in vec3 Position;

layout (location = 2) in vec2 UV;

out vec2 uv_coord;
out vec4 _Col;

void main()
{
    gl_Position = vec4(Position, 1.0);
	uv_coord = UV;
    _Col = vec4(0.5, 0.1, 0.7, 1.0);
}