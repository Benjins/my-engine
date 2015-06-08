#include "../header/int/Drawcall.h"
#include "../header/int/GameObject.h"
#include "../header/int/Mat4.h"
#include "../header/int/Material.h"
#include "../header/int/Texture.h"
#include "../header/int/Scene.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

DrawCall::DrawCall(GameObject* _obj){
	obj = _obj;
	int size = _obj->mesh->faces.size() * 3;
	vertCount = size;
	vector<Vector3> Vertices;
	vector<Vector3> Normals;
	vector<Vector2> uvCoords;
	vector<Vector3> tangentData;

    for(int i = 0; i < _obj->mesh->faces.size(); i++){
		Face face = _obj->mesh->faces[i];
		
		Vertices.push_back(_obj->mesh->vertices[face.v2].position);
		Vertices.push_back(_obj->mesh->vertices[face.v1].position);
		Vertices.push_back(_obj->mesh->vertices[face.v0].position);

		Normals.push_back(_obj->mesh->vertices[face.v2].normal);
		Normals.push_back(_obj->mesh->vertices[face.v1].normal);
		Normals.push_back(_obj->mesh->vertices[face.v0].normal);

		tangentData.push_back(_obj->mesh->vertices[face.v2].tangent);
		tangentData.push_back(_obj->mesh->vertices[face.v1].tangent);
		tangentData.push_back(_obj->mesh->vertices[face.v0].tangent);
		//cout << Vertices[i].Magnitude() << endl;
		
		uvCoords.push_back(face.uv2);
		uvCoords.push_back(face.uv1);
		uvCoords.push_back(face.uv0);
	}

	//vector<Vector3> tangentData;
	//CalculateTangents(_obj->mesh, tangentData);

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
}

void DrawCall::Draw() const{
	Material* material = obj->material;
	glUseProgram(material->shaderProgram);

	GLfloat uvMatrix[9] = { material->uvScale.x, 0, material->uvOffset.x,
							0, material->uvScale.y, material->uvOffset.y,
							0,                      0,                 1};

	glUniformMatrix3fv(material->GetUniformByName("_uvMatrix"), 1, GL_TRUE, uvMatrix);

	glUniformMatrix4fv(material->GetUniformByName("_objectMatrix"), 1, GL_TRUE,  &obj->transform.LocalToGlobalMatrix().m[0][0]);
	glUniform1i(glGetUniformLocation(material->shaderProgram, "_mainTex"), 0);
	if(material->bumpMap != nullptr){
		glUniform1i(glGetUniformLocation(material->shaderProgram, "_bumpMap"), 1);
	}
	else{
		glUniform1i(glGetUniformLocation(material->shaderProgram, "_bumpMap"), -1);
	}
	
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
}

void CalculateTangents(Model* model, vector<Vector3>& outTangentData){

}

