#version 330

layout (location = 0) in vec3 Position;

layout (location = 2) in vec2 UV;

out vec2 uv_coord;

uniform mat4 _perspMatrix;
uniform mat4 _cameraMatrix;
uniform mat4 _objectMatrix;

void main()
{
    gl_Position =  _perspMatrix * _cameraMatrix * _objectMatrix * vec4(Position, 1.0);
	uv_coord = UV;
}