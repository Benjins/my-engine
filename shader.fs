#version 130
#define MAX_NUM_LIGHTS 6

out vec4 FragColor;

in vec2 uv_coord;
in vec3 normal;
in vec3 pos;
in vec3 tangent;

uniform sampler2D _mainTex;
uniform sampler2D _bumpMap;
uniform vec4 _color;

uniform vec3 lightVectors[MAX_NUM_LIGHTS];
uniform int lightIsDirectional[MAX_NUM_LIGHTS];
uniform int numLights;

void main()
{
	if(abs(dot(tangent, normal)) > 0.01){discard;}
	
	vec4 texCol  = texture2D(_mainTex, uv_coord.st);
	vec4 bumpCol = texture2D(_bumpMap, uv_coord.st);
	texCol.a = 1.0;
	vec4 color = mix(mix(texCol, bumpCol, 0.5), _color, 0.5);
	
	float lighting = 0.05;
	for(int i = 0; i < numLights; i++){
		if(lightIsDirectional[i] != 0){
			lighting += (dot(normal, lightVectors[i]) + 1)/2;
		}
		else{
			vec3 difference = lightVectors[i] - pos;
			float distance = length(difference);
			float normalDot = dot(normal, difference/distance);
			float clampedDist = min(distance, 5);
			float thisLighting = max(0, (normalDot+1)/2 * (5 - clampedDist));
			lighting += thisLighting;
		}
	}
	
	FragColor = color * lighting;
}
