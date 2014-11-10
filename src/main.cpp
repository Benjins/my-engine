#include "../header/int/Scene.h"
#include "../header/int/GameObject.h"
#include "../header/int/Component.h"
#include "../header/int/Mat4.h"
#include "../header/int/Collider.h"
#include "../test/testing.h"
#include "../header/int/Vector4.h"
#include <iomanip>

#ifdef TESTING
#if defined(_WIN32) || defined(_WIN64)
#  define _CRTDBG_MAP_ALLOC
#  define _CRTDBG_MAP_ALLOC_NEW
#  include <crtdbg.h>
#endif
#endif
#if defined(MEM_CHECK) || defined(TESTING)
#  include <assert.h>
#endif


#include <iostream>
using std::cout; using std::endl;

int main(int argc, char** argv){

	/*
	Mat4x4 mat;
	mat.SetRow(0, Vector4(Rotate(X_AXIS,Quaternion(Vector3(Y_AXIS),1)), 0));
	mat.SetRow(1, Vector4(0,1,0,0));
	mat.SetRow(2, Vector4(Rotate(Z_AXIS,Quaternion(Vector3(Y_AXIS),1)), 0));
	mat.SetRow(3, Vector4(0,0,0,1));

	//test mat transforming some axes
	Vector3 transVec = mat * Vector3(1,0,0);
	transVec.Print();

	Rotate(X_AXIS,Quaternion(Vector3(Y_AXIS),1)).Print();
	Rotate(Z_AXIS,Quaternion(Vector3(Y_AXIS),1)).Print();
	return 0;*/


#if (defined(_WIN32) || defined(_WIN64)) && (defined(TESTING) || defined(MEM_CHECK))
	//Windows memory leak checking
	_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
	_CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
	_CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT );
	_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
	_CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT );
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

#ifdef TESTING 

	int retVal = RunAllTests();
#else
	Scene& x = Scene::getInstance(argc, argv);

	x.Init();

	x.Start();
	
	return 0;
#endif

#if (defined(_WIN32) || defined(_WIN64)) && (defined(MEM_CHECK) || defined(TESTING))
	assert(_CrtCheckMemory());
#endif

#ifdef TESTING 
	return retVal;
#else

#endif
}