#version 330

out vec4 FragColor;

in vec2 uv_coord;

uniform sampler2D _mainTex;

void main()
{
    FragColor = texture2D(_mainTex, uv_coord.st);
}
