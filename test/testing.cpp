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