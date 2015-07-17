#version 130

#define MAX_BONES_PER_VERT 4
#define MAX_BONE_COUNT 16

in vec3 Position;
in vec2 UV;
in vec3 _normal;
in vec3 _tangent;
in int boneIndices[MAX_BONES_PER_VERT];
in float boneWeights[MAX_BONES_PER_VERT];
in int boneCount;

out vec2 uv_coord;
out vec3 normal;
out vec3 pos;
out vec3 tangent;

uniform mat3 _uvMatrix;
uniform mat4 _perspMatrix;
uniform mat4 _cameraMatrix;
uniform mat4 _objectMatrix;

uniform mat4 _armatureMatrices;

void main()
{
	vec3 vertPos = Position;
	vec2 uvPos = UV;
	mat4 boneMatrix = mat4(1.0);
	
	int _boneCount = 2;//  min(2, max(0, boneCount));
	for(int i = 0; i < _boneCount; i++){
		//mat4 singleBoneMat = _armatureMatrices[boneIndices[i]] * boneWeights[i];
		//boneMatrix += singleBoneMat;
	}
	
	//boneMatrix[0] = vec4(1, 0, 0, 0);
	//boneMatrix[2] = vec4(0, 1, 0, 0);
	//boneMatrix[1] = vec4(0, 0, 1, 0);
	//boneMatrix[3] = vec4(0, 0, 0, 1);
	
	mat4 armatureMat = _armatureMatrices ;// [0];
	
	if(armatureMat[0][0] != 1){
		uvPos.y += 0.00000001;
	}
	
	pos = (_objectMatrix * vec4(vertPos, 1.0)).xyz;
    gl_Position =  _perspMatrix * _cameraMatrix * vec4(pos, 1.0);
	uv_coord = (_uvMatrix * vec3(uvPos, 1.0)).xy;
	normal  = (_objectMatrix * vec4(_normal, 0.0)).xyz;
	tangent = (_objectMatrix * vec4(_tangent, 0.0)).xyz;
}
