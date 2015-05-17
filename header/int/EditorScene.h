#ifndef EDITORSCENE_H
#define EDITORSCENE_H

#include "Scene.h"

struct EditorScene : Scene{

	EditorScene();

	EditorScene(int argc, char** argv);

	SC_Transform editorCamera;

	static EditorScene& getInstance(){
		return getInstance(0,NULL);
	}
	static EditorScene& getInstance(int argc, char** argv){
		static EditorScene instance = EditorScene(argc, argv);
		return instance;
	}

	virtual void Start();

	void EditorUpdate();
};


static void OnEditorMouseFunc(int button, int state, int x, int y);

static void OnEditorPassiveMouseFunc(int x, int y);

static void OnEditorKeyFunc(unsigned char key, int x, int y);

static void OnEditorKeyUpFunc(unsigned char key, int x, int y);

#endif