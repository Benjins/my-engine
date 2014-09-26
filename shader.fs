#version 330

out vec4 FragColor;

in vec4 _Col;
in vec2 uv_coord;

uniform sampler2D _mainTex;

void main()
{
    FragColor = _Col;
}
