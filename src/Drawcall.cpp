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

    for(int i = 0; i < _obj->mesh->faces.size(); i++){
		Face face = _obj->mesh->faces[i];
		
		Vertices.push_back(_obj->mesh->vertices[face.v2].position);
		Vertices.push_back(_obj->mesh->vertices[face.v1].position);
		Vertices.push_back(_obj->mesh->vertices[face.v0].position);

		Normals.push_back(_obj->mesh->vertices[face.v2].normal);
		Normals.push_back(_obj->mesh->vertices[face.v1].normal);
		Normals.push_back(_obj->mesh->vertices[face.v0].normal);

		//cout << Vertices[i].Magnitude() << endl;
		
		uvCoords.push_back(face.uv2);
		uvCoords.push_back(face.uv1);
		uvCoords.push_back(face.uv0);
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
	
	glUniform1i(_obj->material->GetUniformByName("_mainTex"), 0);
}

void DrawCall::Draw() const{
	Material* material = obj->material;
	glUseProgram(material->shaderProgram);

	glUniformMatrix4fv(material->GetUniformByName("_objectMatrix"), 1, GL_TRUE,  &obj->transform.LocalToGlobalMatrix().m[0][0]);
	glUniform3fv(material->GetUniformByName("lightDirecton"), 1, (float*)&obj->scene->lightDir);
	
	glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertices);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvs);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normals);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	

	material->mainTexture->Bind(GL_TEXTURE0);
	
	glDrawArrays(GL_TRIANGLES, 0, vertCount);
	
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}
