#ifdef TESTING

#include "../header/int/Scene.h"
#include "../header/int/GameObject.h"
#include "../header/int/Component.h"
#include "../header/int/Material.h"
#include "../header/int/Texture.h"
#include "../header/int/Mat4.h"
#include "../header/int/Collider.h"
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

	SC_Transform transTest1;
	SC_Transform transTest1_p;
	transTest1.parent = &transTest1_p;
	transTest1.position = X_AXIS;
	transTest1_p.rotation = Quaternion(Y_AXIS, 3.14159265358979f/2);
	Vector3 globalTest1 = transTest1.GlobalPosition();
	
	AssertApprox(globalTest1.x,0,"global position test 1 x-value");
	AssertApprox(globalTest1.y,0,"global position test 1 y-value");
	AssertApprox(globalTest1.z,1,"global position test 1 z-value");

	transTest1_p.rotation = Quaternion(Y_AXIS, 3.14159265358979f);
	transTest1_p.position = Z_AXIS * 3;
	Vector3 globalTest2 = transTest1.GlobalPosition();
	
	AssertApprox(globalTest2.x,-1,"global position test 2 x-value");
	AssertApprox(globalTest2.y,0,"global position test 2 y-value");
	AssertApprox(globalTest2.z,3,"global position test 2 z-value");

	{
		CrtCheckMemory memCheck;
		BoxCollider* boxCol1 = new BoxCollider();
		boxCol1->position = Vector3(0,0,0);
		boxCol1->size = Vector3(1,1,1);
		SphereCollider* sphereCol1 = new SphereCollider();
		sphereCol1->position = Vector3(1.2f, 0, 0);
		sphereCol1->radius = 0.6f;
		AssertEqual<bool>(boxCol1->CollisionWith(sphereCol1).collide, true, "boxcol1 collides with spherecol1");

		sphereCol1->position = Vector3(2.0f, 2.0f, 2.0f);
		AssertEqual<bool>(boxCol1->CollisionWith(sphereCol1).collide, false, "boxcol1 doesn't collide with spherecol1 after it's moved");

		boxCol1->size = Vector3(3,1.8f,1.8f);
		AssertEqual<bool>(boxCol1->CollisionWith(sphereCol1).collide, true, "boxcol1 does collide with spherecol1 after box is grown");

		SphereCollider* sphereCol2 = new SphereCollider();
		sphereCol2->position = Vector3(2.0f,2.0f,2.8f);
		sphereCol2->radius = 0.21f;
		AssertEqual<bool>(sphereCol2->CollisionWith(sphereCol1).collide, true, "sphereCol2 does collide with spherecol1");

		sphereCol2->radius = 0.19f;
		AssertEqual<bool>(sphereCol2->CollisionWith(sphereCol1).collide, false, "sphereCol2 does not collide with spherecol1 atfer it shrinks");

		BoxCollider* boxCol2 = new BoxCollider();
		boxCol2->position = Vector3(4.01f, 3, 3);
		boxCol2->size = Vector3(1,1,1);
		AssertEqual<bool>(boxCol2->CollisionWith(boxCol1).collide, false, "boxCol1 does not collide with boxCol2 at first");

		boxCol2->position = Vector3(2, 2, 2);
		boxCol2->size = Vector3(1.2f,1.1f,1.1f);
		AssertEqual<bool>(boxCol2->CollisionWith(boxCol1).collide, true, "boxCol1 does collide with boxCol2 when box2 grows");

		delete boxCol1;
		delete boxCol2;
		delete sphereCol1;
		delete sphereCol2;
	}

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

	//Scoping
	{
		CrtCheckMemory memCheck;

		GameObject* obj = new GameObject();
		obj->AddComponent<Component>();
		obj->AddComponent<Component>();
		obj->AddComponent<Component>();
		obj->AddMesh("test.obj");
		obj->material = new Material();
		obj->material->mainTexture = new Texture(GL_TEXTURE_2D, "texture.bmp");
		delete obj;
	}

	//--------------------------
	//Performance testing
	PerformanceTest(BoxSpherePerformance,20000, "BoxSphereCollision");
	PerformanceTest(SphereSpherePerformance,20000, "SphereSpherePerformance");
	PerformanceTest(BoxBoxPerformance,20000, "BoxBoxPerformance");


	//---------------------------


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

static void BoxBoxPerformance(){
	BoxCollider box1, box2;
	box1.position = Vector3(2,5,-2);
	box1.size = Vector3(3,2,8);
	box2.position = Vector3(-2,4,-3);
	box2.size = Vector3(1,2,1);

	Collision x = DetectCollision(&box1, &box2);
}

static void BoxSpherePerformance(){
	BoxCollider box1;
	SphereCollider sphere1;
	box1.position = Vector3(2,5,-2);
	box1.size = Vector3(3,2,8);
	sphere1.position = Vector3(-2,4,-3);
	sphere1.radius = 0.5f;

	Collision x = DetectCollision(&sphere1, &box1);
}

static void SphereSpherePerformance(){
	SphereCollider sphere1, sphere2;
	sphere2.position = Vector3(2,5,-2);
	sphere2.radius = 1.2f;
	sphere1.position = Vector3(-2,4,-3);
	sphere1.radius = 0.5f;

	Collision x = DetectCollision(&sphere1, &sphere2);
}

void AssertApprox(float expected, float actual, string error, float maxDifference){
	testCount++;

	float difference = actual - expected;

	if(difference > maxDifference || difference < -maxDifference){
		cerr << "AssertApprox Failed with difference " << difference <<  "  Msg: " << error << endl;
	}
	else{
		passedTests++;
	}
}

double PerformanceTest(void (*func) (void), int count, string name){
	int trueCount = count;

	//If you're stupid enough to call with count < 0, I'm sorry.
	if(trueCount < 0){
		trueCount = 0;
	}

	clock_t start = clock();

	for(int i = 0; i < trueCount; i++){
		func();
	}

	clock_t end = clock();

	double timeElapsed = (double(end-start))/CLOCKS_PER_SEC*1000;

	cout << "\nPerformance test for: " << name << ", ran " << trueCount << " times in " << timeElapsed << " ms.\n";
	return timeElapsed;
}

#endif