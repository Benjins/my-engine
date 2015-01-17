#version 130

out vec4 FragColor;

in vec2 uv_coord;

uniform sampler2D _mainTex;
uniform vec4 _color;

void main()
{
	vec4 texCol = texture2D(_mainTex, uv_coord.st);
	texCol.a = 1.0;
    FragColor = mix(texCol, _color, 0.5);
}
