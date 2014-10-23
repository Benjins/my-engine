#ifndef TESTING_H
#define TESTING_H

#ifdef TESTING

#include <string>
#include <iostream>

using std::string; 

static int testCount = 0;
static int passedTests = 0;

int RunAllTests();

template<typename T>
void AssertEqual(T actual, T expected, string error){
	testCount++;

	if(actual == expected){
		passedTests++;
	}
	else{
		cerr << "Test Failed!  Msg: " << error << endl;
	}
}

void AssertTrue(bool check, string error);

void AssertApprox(float actual, float expected, string error, float maxDifference = 0.0001f);

#endif

#endif