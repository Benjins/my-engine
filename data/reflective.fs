#version 130
#define MAX_NUM_LIGHTS 6

out vec4 FragColor;

in vec2 uv_coord;
in vec3 normal;
in vec3 pos;
in vec3 tangent;
in vec3 cameraVec;

uniform float reflectiveLevel = 0.9;

uniform sampler2D _mainTex;
uniform sampler2D _bumpMap;
uniform samplerCube _cubeMap;

void main()
{
	vec4 texCol  = texture2D(_mainTex, uv_coord.st);
	vec4 bumpCol = texture2D(_bumpMap, uv_coord.st);
	
	vec3 actualTangent = normalize(tangent - dot(tangent, normal) * normal);
	bumpCol = bumpCol * 2.0 - vec4(1.0,1.0,1.0,0.0);
	vec3 usedNormal = normal * bumpCol.b + actualTangent * bumpCol.g + cross(normal, actualTangent) * bumpCol.r;
	
	vec3 normalProj = dot(cameraVec, usedNormal) * usedNormal;
	vec3 sampleVec = cameraVec - (2 * normalProj);
	
	vec4 reflectiveSample = texture(_cubeMap, sampleVec);
	
	FragColor = mix(texCol, reflectiveSample, reflectiveLevel);
}
