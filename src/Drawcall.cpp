#include "../header/int/Drawcall.h"
#include "../header/int/GameObject.h"
#include "../header/int/Mat4.h"
#include "../header/int/Material.h"
#include "../header/int/Texture.h"
#include "../header/int/Scene.h"
#include "../header/int/Armature.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <algorithm>

using std::max;

DrawCall::DrawCall(GameObject* _obj){
	obj = _obj;
	int size = static_cast<int>(_obj->mesh->faces.size() * 3);
	vertCount = size;
	vector<Vector3> Vertices;
	vector<Vector3> Normals;
	vector<Vector2> uvCoords;
	vector<Vector3> tangentData;
	vector<int> boneCountsData;
	vector<int> boneIndicesData;
	vector<float> boneWeightsData;

    for(int i = 0; i < _obj->mesh->faces.size(); i++){
		Face face = _obj->mesh->faces[i];
		
		const Vertex& vert0 = _obj->mesh->vertices[face.v0];
		const Vertex& vert1 = _obj->mesh->vertices[face.v1];
		const Vertex& vert2 = _obj->mesh->vertices[face.v2];

		Vertices.push_back(vert2.position);
		Vertices.push_back(vert1.position);
		Vertices.push_back(vert0.position);

		Normals.push_back(vert2.normal);
		Normals.push_back(vert1.normal);
		Normals.push_back(vert0.normal);

		tangentData.push_back(vert2.tangent);
		tangentData.push_back(vert1.tangent);
		tangentData.push_back(vert0.tangent);
		//cout << Vertices[i].Magnitude() << endl;
		
		uvCoords.push_back(face.uv2);
		uvCoords.push_back(face.uv1);
		uvCoords.push_back(face.uv0);

		if(_obj->mesh->armature != nullptr){
			boneCountsData.push_back(min((int)vert2.numBones, 4));
			boneCountsData.push_back(min((int)vert1.numBones, 4));
			boneCountsData.push_back(min((int)vert0.numBones, 4));

			for(int i = 0; i < MAX_BONES_PER_VERT; i++){
				boneIndicesData.push_back(vert2.boneIndices[i]);
				boneWeightsData.push_back(vert2.boneWeights[i]);
			}
			for(int i = 0; i < MAX_BONES_PER_VERT; i++){
				boneIndicesData.push_back(vert1.boneIndices[i]);
				boneWeightsData.push_back(vert1.boneWeights[i]);
			}
			for(int i = 0; i < MAX_BONES_PER_VERT; i++){
				boneIndicesData.push_back(vert0.boneIndices[i]);
				boneWeightsData.push_back(vert0.boneWeights[i]);
			}
		}
	}

 	glGenBuffers(1, &vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3)*Vertices.size(), &(Vertices[0]), GL_STATIC_DRAW);
	
	glGenBuffers(1, &uvs);
	glBindBuffer(GL_ARRAY_BUFFER, uvs);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2)*uvCoords.size(), &(uvCoords[0]), GL_STATIC_DRAW);

	glGenBuffers(1, &normals);
	glBindBuffer(GL_ARRAY_BUFFER, normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3)*Normals.size(), &(Normals[0]), GL_STATIC_DRAW);

	glGenBuffers(1, &tangents);
	glBindBuffer(GL_ARRAY_BUFFER, tangents);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3)*tangentData.size(), &(tangentData[0]), GL_STATIC_DRAW);

	if(_obj->mesh->armature != nullptr){
		glGenBuffers(1, &boneCount);
		glBindBuffer(GL_ARRAY_BUFFER, boneCount);
		glBufferData(GL_ARRAY_BUFFER, sizeof(int)*boneCountsData.size(), &(boneCountsData[0]), GL_STATIC_DRAW);

		glGenBuffers(1, &boneIndices);
		glBindBuffer(GL_ARRAY_BUFFER, boneIndices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(int)*boneIndicesData.size(), &(boneIndicesData[0]), GL_STATIC_DRAW);

		glGenBuffers(1, &boneWeights);
		glBindBuffer(GL_ARRAY_BUFFER, boneWeights);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*boneWeightsData.size(), &(boneWeightsData[0]), GL_STATIC_DRAW);
	}
}

void DrawCall::Draw() const{
	Material* material = obj->material;
	glUseProgram(material->shaderProgram);

	GLfloat uvMatrix[9] = { material->uvScale.x, 0, material->uvOffset.x,
							0, material->uvScale.y, material->uvOffset.y,
							0,                      0,                 1};

	GLenum erroR = glGetError();

	glUniformMatrix3fv(material->GetUniformByName("_uvMatrix"), 1, GL_TRUE, uvMatrix);

	GLenum erroQ = glGetError();

	glUniformMatrix4fv(material->GetUniformByName("_objectMatrix"), 1, GL_TRUE,  &obj->transform.LocalToGlobalMatrix().m[0][0]);
	glUniform1i(glGetUniformLocation(material->shaderProgram, "_mainTex"), 0);
	if(material->bumpMap != nullptr){
		glUniform1i(glGetUniformLocation(material->shaderProgram, "_bumpMap"), 1);
	}
	else{
		glUniform1i(glGetUniformLocation(material->shaderProgram, "_bumpMap"), -1);
	}

	GLenum erroX = glGetError();

	vector<Mat4x4> boneMatrices;
	if(obj->mesh->armature != nullptr){
		obj->mesh->armature->GetBoneMatrices(boneMatrices);
	}

	GLenum erroC = glGetError();
	
	glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertices);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvs);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normals);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, tangents);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);

	GLenum erroV = glGetError();

	if(obj->mesh->armature != nullptr){
		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, boneIndices);
		glVertexAttribIPointer(4, 4, GL_INT, 0, 0);

		glEnableVertexAttribArray(5);
		glBindBuffer(GL_ARRAY_BUFFER, boneWeights);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_TRUE, 0, 0);

		glEnableVertexAttribArray(6);
		glBindBuffer(GL_ARRAY_BUFFER, boneCount);
		glVertexAttribIPointer(6, 1, GL_INT, 0, 0);

		GLenum erro1 = glGetError();

		GLint armatureUniformLoc = glGetUniformLocation(material->shaderProgram, "_armatureMatrices");
		glUniformMatrix4fv(armatureUniformLoc, boneMatrices.size(), GL_TRUE, &(boneMatrices[0].m[0][0]));

		erro1 = glGetError();
	}

	if(material->mainTexture != nullptr){
		material->mainTexture->Bind(GL_TEXTURE0);
	}

	if(material->bumpMap != nullptr){
		material->bumpMap->Bind(GL_TEXTURE1);
	}
	
	glDrawArrays(GL_TRIANGLES, 0, vertCount);
	
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);

	if(obj->mesh->armature != nullptr){
		GLuint armatureUniformLoc = glGetUniformLocation(material->shaderProgram, "_armatureMatrices");
		float uniformData[MAX_BONE_COUNT * 16];
		glGetUniformfv(obj->material->shaderProgram, armatureUniformLoc, uniformData);

		glDisableVertexAttribArray(4);
		glDisableVertexAttribArray(5);
		glDisableVertexAttribArray(6);
	}
}

void CalculateTangents(Model* model, vector<Vector3>& outTangentData){

}

