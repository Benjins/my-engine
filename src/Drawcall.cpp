#include "../header/int/Drawcall.h"
#include "../header/int/Material.h"
#include "../header/int/Texture.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

DrawCall::DrawCall(const Model& model, Material* mat){
	material = mat;
	int size = model.faces.size() * 3;
	vertCount = size;
	vector<Vector3> Vertices;
	vector<Vector2> uvCoords;

    for(int i = 0; i < model.faces.size(); i++){
		Face face = model.faces[i];
		
		Vertices.push_back(model.vertices[face.v2].position);
		Vertices.push_back(model.vertices[face.v1].position);
		Vertices.push_back(model.vertices[face.v0].position);

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
	
	glUniform1i(mat->textureUniform, 0);
}

void DrawCall::Draw() const{
	glUseProgram(material->shaderProgram);
	
	glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertices);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	//glEnableVertexAttribArray(1);
	//glBindBuffer(GL_ARRAY_BUFFER, colors);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, uvs);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	
	material->mainTexture->Bind(GL_TEXTURE0);
	
	//cout << "Vertcount: " << vertCount << endl;
	glDrawArrays(GL_TRIANGLES, 0, vertCount);
	
	glDisableVertexAttribArray(0);
	//glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}