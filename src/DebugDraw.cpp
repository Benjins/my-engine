#include "../header/int/DebugDraw.h"
#include "../header/int/ResourceManager.h"
#include "../header/int/Material.h"
#include "../header/int/Macros.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/freeglut.h>
#endif

void DebugDraw::Render(MaterialManager* resources){
	
	if(mat == nullptr){
		mat = resources->GetMaterialByName("color");
	}

	glUseProgram(mat->shaderProgram);

	glUniform4f(glGetUniformLocation(mat->shaderProgram, "_color"), 1, 1, 1, 1);

	for(auto& iter : debugCubes){
		Vector3 points[] = {iter.center + iter.forward + iter.right + iter.up, 
							iter.center + iter.forward + iter.right - iter.up, 
							iter.center + iter.forward - iter.right - iter.up, 
							iter.center + iter.forward - iter.right + iter.up, 
							iter.center + iter.forward + iter.right + iter.up, 
							iter.center - iter.forward + iter.right + iter.up, 
							iter.center - iter.forward + iter.right - iter.up, 
							iter.center - iter.forward - iter.right - iter.up, 
							iter.center - iter.forward - iter.right + iter.up,
							iter.center - iter.forward + iter.right + iter.up,
							iter.center - iter.forward + iter.right - iter.up,
							iter.center + iter.forward + iter.right - iter.up,
							iter.center + iter.forward - iter.right - iter.up,
							iter.center - iter.forward - iter.right - iter.up,
							iter.center - iter.forward - iter.right + iter.up,
							iter.center + iter.forward - iter.right + iter.up};
		
		glBegin(GL_LINE_STRIP);
		{
			for(int i = 0; i < ARRAY_COUNT(points); i++){
				glVertex3fv((float*)&points[i]);
			}
		}
		glEnd();
	}

	debugCubes.clear();
	debugLines.clear();
	debugSpheres.clear();
}

void DebugDraw::Cube(const Vector3& position, const Vector3& size, bool isWire/*=true*/){
	DebugCube cube = {position, Z_AXIS * size.z, X_AXIS*size.x, Y_AXIS*size.y, isWire};	
	debugCubes.push_back(cube);
}

void DebugDraw::Cube(const Vector3& position, const Vector3& up, const Vector3& right, const Vector3& forward, bool isWire/*=true*/){
	DebugCube cube = {position, forward, right, up, isWire};	
	debugCubes.push_back(cube);
}
void DebugDraw::Line(const Vector3& from, const Vector3& to){}
void DebugDraw::Sphere(const Vector3& center, float radius, bool isWire/*=true*/){}

