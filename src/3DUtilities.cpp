#include "../header/int/3DUtilities.h"

Quaternion RotateTowards(const Quaternion& from, const Quaternion& to, float maxDegreesDelta){
	maxDegreesDelta *= (1.0f/60);

	float distance = (from - to).Magnitude();
	if(distance < maxDegreesDelta){
		return to;
	}

	float weight = maxDegreesDelta/distance;
	return from * (1 - weight) + to * weight;
}
