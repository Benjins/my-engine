#include "../header/int/Scene.h"
#include "../header/int/GameObject.h"
#include "../header/int/Component.h"
#include "../header/int/Mat4.h"
#include "../test/testing.h"
#include <iomanip>

#ifdef TESTING
#if defined(_WIN32) || defined(_WIN64)
#  define _CRTDBG_MAP_ALLOC
#  define _CRTDBG_MAP_ALLOC_NEW
#  include <crtdbg.h>
#  include <assert.h>
#endif
#endif

#include <iostream>
using std::cout; using std::endl;

int main(int argc, char** argv){

#ifdef TESTING
#if defined(_WIN32) || defined(_WIN64)
	//Windows memory leak checking
	_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
	_CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
	_CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT );
	_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
	_CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT );
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	//int* y = new int[6];

	int retVal = RunAllTests();

	//Scene x = Scene::getInstance(argc, argv);
	//x.Init();
	//x.Start();

	//int* z = new int[1];

#if defined(_WIN32) || defined(_WIN64)
	assert(_CrtCheckMemory());
#endif

	return retVal;
#else


	Scene& x = Scene::getInstance(argc, argv);

	//x.Init();

	x.Start();
	
	return 0;
#endif
}