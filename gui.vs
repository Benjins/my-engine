#version 130

attribute vec3 Position;

out vec2 uv_coord;

void main()
{
    gl_Position = vec4(Position, 1.0);
	uv_coord = vec2((Position.x+1)/2, (Position.y+1)/2);
}
