#include "../header/int/Scene.h"
#include "../header/int/GameObject.h"
#include "../header/int/Component.h"
#include "../header/int/Mat4.h"
#include "../test/testing.h"
#include <iomanip>

#ifdef TESTING
	#define CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif

#include <iostream>
using std::cout; using std::endl;

int main(int argc, char** argv){

#ifdef TESTING
	_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
	_CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
	_CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT );
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	int retVal = RunAllTests();

	//Scene x = Scene::getInstance(argc, argv);
	//x.Init();
	//x.Start();

	//delete x;
	return retVal;
#else


	Scene x = Scene::getInstance(argc, argv);

	//x.Init();

	x.Start();
	
	return 0;
#endif
}