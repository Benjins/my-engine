#version 130

out vec4 FragColor;

in vec2 uv_coord;
uniform sampler2D _guiTex;

void main()
{
    FragColor = vec4(1.0, uv_coord.x, uv_coord.y, 0.5);
}
