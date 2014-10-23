#ifdef TESTING

#include "../header/int/Scene.h"
#include "../header/int/GameObject.h"
#include "../header/int/Component.h"
#include "../header/int/Material.h"
#include "../header/int/Mat4.h"
#include "testing.h"

#include <string>
#include <iostream>
using std::cerr; using std::endl; using std::string; using std::cout;

CrtCheckMemory::CrtCheckMemory(){
#if defined(_WIN32) || defined(_WIN64)
    _CrtMemCheckpoint(&state1);
#endif
  }

 CrtCheckMemory::~CrtCheckMemory(){
#if defined(_WIN32) || defined(_WIN64)
    _CrtMemCheckpoint(&state2);
    // using google test you can just do this.
	int diff = _CrtMemDifference( &state3, &state1, &state2);
	AssertEqual<int>(0, diff, "Memory leak!");
    // else just do this to dump the leaked blocks to stdout.
    if( _CrtMemDifference( &state3, &state1, &state2) ){
      _CrtMemDumpStatistics( &state3 );
	}
#endif
  }

int RunAllTests(){
	testCount = 0;
	passedTests = 0;


	//Put tests here:-------------

	AssertEqual<Quaternion>(QUAT_IDENTITY, QUAT_IDENTITY, "identity crisis");
	AssertEqual<Vector3>(Rotate(X_AXIS, Quaternion(1,0,0,0)), X_AXIS, "x-axis stays the same");
	AssertEqual<Vector3>(Rotate(X_AXIS, Quaternion(X_AXIS, 20)), X_AXIS, "x-axis stays the same when rotated about itself");
	//AssertEqual<Vector3>(Rotate(X_AXIS, Quaternion(X_AXIS, 3.8)), X_AXIS, "x-axis stays the same when rotated about itself");
	
	Vector3 rotatedX1 = Rotate(X_AXIS, Quaternion(Y_AXIS, 3.14159265358979f));
	AssertApprox(rotatedX1.x, -1, "rotate X-axis by 180 deg around y-axis, x value");
	AssertApprox(rotatedX1.y,  0, "rotate X-axis by 180 deg around y-axis, y value");
	AssertApprox(rotatedX1.z,  0, "rotate X-axis by 180 deg around y-axis, z value");

	Vector3 rotatedX2 = Rotate(X_AXIS, Quaternion(Z_AXIS, 3.14159265358979f));
	AssertApprox(rotatedX2.x, -1, "rotate X-axis by 180 deg around z-axis, x value");
	AssertApprox(rotatedX2.y,  0, "rotate X-axis by 180 deg around z-axis, y value");
	AssertApprox(rotatedX2.z,  0, "rotate X-axis by 180 deg around z-axis, z value");

	vector<string> splitTest1 = SplitStringByDelimiter("1%2%3%4%5","%");
	AssertEqual<int>(splitTest1.size(), 5, "splitTest1 size check");
	for(int i = 0; i < splitTest1.size(); i++){
		AssertEqual<string>(splitTest1[i],to_string(i+1), "splitTest1 elem " + to_string(i) + " check");
	}

	vector<string> splitTest2 = SplitStringByDelimiter("%%%%%%%%","%");
	AssertEqual<int>(splitTest2.size(), 9, "splitTest2 size check");
	for(int i = 0; i < splitTest2.size(); i++){
		AssertEqual<string>(splitTest2[i], "", "splitTest2 blanks check");
	}

	AssertEqual<string>(TrimWhitespace(""), "", "TrimWhitespace empty check.");
	AssertEqual<string>(TrimWhitespace("AAFAAGDG"), "AAFAAGDG", "TrimWhitespace no whitespace check.");
	AssertEqual<string>(TrimWhitespace("   AAFAAGDG"), "AAFAAGDG", "TrimWhitespace leading whitespace check.");
	AssertEqual<string>(TrimWhitespace("   AAFAAGDG    "), "AAFAAGDG", "TrimWhitespace leading and trailing whitespace check.");
	AssertEqual<string>(TrimWhitespace(" \n\t  \t\t\t  \t\n\n   AAFAAGDG    "), "AAFAAGDG", "TrimWhitespace mixed whitespace check.");
	AssertEqual<string>(TrimWhitespace(" \n  A  AF   AAGD  \tG \t  "), "A  AF   AAGD  \tG", "TrimWhitespace interior whitespace check.");

	vector<string> uniformNames1 = GetUniformNames("uniform sampler2D _mainTex;");
	AssertEqual<int>(uniformNames1.size(), 1, "Number of uniforms found in uniformName1");
	AssertEqual<string>(uniformNames1[0], "_mainTex", "uniformName1 first uniform's name check");
	//cout << "uniformNames1[0]:" << uniformNames1[0] << ":_mainTex:" << endl;

	vector<string> uniformNames2 = GetUniformNames("\t  uniform   sampler2D   _mainTex;  ");
	AssertEqual<int>(uniformNames2.size(), 1, "Number of uniforms found in uniformName2");
	AssertEqual<string>(uniformNames2[0], "_mainTex", "uniformName2 first uniform's name check");

	vector<string> uniformNames3 = GetUniformNames("\t  uniform   sampler2D   _mainTex;  \n\t uniform vec4 position;");
	AssertEqual<int>(uniformNames3.size(), 2, "Number of uniforms found in uniformName3");
	AssertEqual<string>(uniformNames3[0], "_mainTex", "uniformName3 first uniform's name check");
	AssertEqual<string>(uniformNames3[1], "position", "uniformName3 second uniform's name check");
	
	vector<string> uniformNames4 = GetUniformNames("#version 330\n\n\n\t  uniform   sampler2D   _mainTex;\n  in vec4 pos; \n out vec4 col;\n\t uniform vec4 position;\nvoid main(){}");
	AssertEqual<int>(uniformNames4.size(), 2, "Number of uniforms found in uniformName4");
	AssertEqual<string>(uniformNames4[0], "_mainTex", "uniformName4 first uniform's name check");
	AssertEqual<string>(uniformNames4[1], "position", "uniformName4 second uniform's name check");

	//Scoping
	{
		CrtCheckMemory memCheck;

		GameObject* obj = new GameObject();
		obj->AddComponent<Component>();
		obj->AddComponent<Component>();
		obj->AddComponent<Component>();
		obj->AddMesh("test.obj");
		delete obj;
	}

	//--------------------------


	cout << endl << passedTests << " of " << testCount << " tests passed.\n";

	//cout << "\nPress Ctrl-C to exit.\n";
	
	if(passedTests == testCount){
		return 0;
	}
	else{
		return -1;
	}
}

void AssertTrue(bool check, string error){
	testCount++;

	if(!check){
		cerr << "Test Failed!  Msg: " << error << endl;
	}
	else{
		passedTests++;
	}
}

void AssertApprox(float actual, float expected, string error, float maxDifference){
	testCount++;

	float difference = actual - expected;

	if(difference > maxDifference || difference < -maxDifference){
		cerr << "AssertApprox Failed with difference " << difference <<  "  Msg: " << error << endl;
	}
	else{
		passedTests++;
	}
}

#endif