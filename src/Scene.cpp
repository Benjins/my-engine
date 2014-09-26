#include "../header/int/Scene.h"
#include <GL/freeglut.h>
#include <GL/glew.h>

Scene::Scene(){
}

Scene::Scene(int argc, char** argv){
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(1280, 720);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("my-engine");

	glutDisplayFunc(RenderScene);
	glutIdleFunc(RenderScene);
	//glutMouseFunc(OnMouse);
	//glutMotionFunc(PassiveMouse);

    // Must be done after glut is initialized!
    GLenum res = glewInit();
    if (res != GLEW_OK) {
      fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
    }
    
    printf("GL version: %s\n", glGetString(GL_VERSION));

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable (GL_DEPTH_TEST);

    glutMainLoop();
}

GameObject& Scene::AddObject(GameObject& obj){
	objects.push_back(obj);
	obj.scene = this;

	if(obj.material != NULL && obj.mesh != NULL){
		drawCalls.push_back(DrawCall(*obj.mesh, obj.material));
	}

	return obj;
}

void Scene::OnUpdate(){
	for(auto iter = objects.begin(); iter != objects.end(); iter++){
		iter->OnUpdate();
	}
}

void Scene::Render(){
	for(auto iter = drawCalls.begin(); iter != drawCalls.end(); iter++){
		iter->Draw();	
	}
}

Scene::~Scene(){
	glutLeaveMainLoop();
}

static void RenderScene(){
	//cout << "\nRendering.";
	Scene::getInstance().Render();
}
