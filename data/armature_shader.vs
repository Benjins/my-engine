#version 130

#define MAX_BONES_PER_VERT 4
#define MAX_BONE_COUNT 16

in vec3 Position;
in vec2 UV;
in vec3 _normal;
in vec3 _tangent;
in ivec4 boneIndices;
in vec4 boneWeights;
in int boneCount;

out vec2 uv_coord;
out vec3 normal;
out vec3 pos;
out vec3 tangent;

uniform mat3 _uvMatrix;
uniform mat4 _perspMatrix;
uniform mat4 _cameraMatrix;
uniform mat4 _objectMatrix;

uniform mat4 _armatureMatrices[MAX_BONE_COUNT];

void main()
{
	vec3 vertPos = Position;
	vec2 uvPos = UV;
	mat4 boneMatrix = mat4(0.0);
	
	for(int i = 0; i < 1; i++){
		mat4 singleBoneMat = _armatureMatrices[boneIndices[i]] * boneWeights[i];
		boneMatrix += singleBoneMat;
	}
	
	mat4 objMat = _objectMatrix;
	objMat[3][3] = boneMatrix[3][3];
	
	pos = (objMat * boneMatrix * vec4(Position, 1.0)).xyz;
    gl_Position =  _perspMatrix * _cameraMatrix * vec4(pos, 1.0);
	uv_coord = (_uvMatrix * vec3(UV, 1.0)).xy;
	normal  = (_objectMatrix * vec4(_normal, 0.0)).xyz;
	tangent = (_objectMatrix * vec4(_tangent, 0.0)).xyz;
}
