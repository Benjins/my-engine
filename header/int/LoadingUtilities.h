#ifndef LOADINGUTILITIES_H
#define LOADINGUTILITIES_H

#include <string>
#include "../ext/Vector3.h"
#include "../ext/Vector2.h"

using std::string;

string EncodeVector3(Vector3 vec);
string EncodeVector2(Vector2 vec);

string EncodeQuaternion(Quaternion quat);

Vector3 ParseVector3(string encoded);

Vector2 ParseVector2(string encoded);

Quaternion ParseQuaternion(string encoded);


#endif
