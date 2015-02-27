#version 130

out vec4 FragColor;

in vec2 uv_coord;
in vec3 normal;

uniform sampler2D _mainTex;
uniform vec4 _color;

void main()
{
	float lighting = dot(normal, vec3(0,0,1));
	vec4 texCol = texture2D(_mainTex, uv_coord.st);
	texCol.a = 1.0;
    vec4 color = mix(texCol, _color, 0.5);
	FragColor = color * lighting;
}
