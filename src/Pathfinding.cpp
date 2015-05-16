#include "../header/int/Pathfinding.h"
#include "../header/int/RaycastHit.h"
#include "../header/int/PhysicsSim.h"
#include "../header/int/Scene.h"

void Pathfinding::HookUpNodes(float maxDistance /*= 20*/){
	for(auto iter = nodes.begin(); iter != nodes.end(); iter++){
		iter->neighbors.clear();
		for(auto neighborIter = nodes.begin(); neighborIter != nodes.end(); neighborIter++){
			if (neighborIter != iter){
				RaycastHit testHit = scene->physicsSim->Raycast(iter->position, neighborIter->position - iter->position);
				if(!testHit.hit || testHit.depth > (neighborIter->position - iter->position).Magnitude()){
					iter->neighbors.push_back(&(*neighborIter));
				}
			}
		}
	}
}

vector<Vector3> Pathfinding::FindPath(Vector3 source, Vector3 destination){
	float closestSourceDist = FLT_MAX;
	float closestDestDist = FLT_MAX;

	PathNode* sourceNode = nullptr;
	PathNode* destNode   = nullptr;

	for(auto iter = nodes.begin(); iter != nodes.end(); iter++){
		float sourceDist = (iter->position - source).MagnitudeSquared();
		float destDist   = (iter->position - destination).MagnitudeSquared();

		if(sourceDist < closestSourceDist){
			closestSourceDist = sourceDist;
			sourceNode = &(*iter);
		}

		if(destDist < closestDestDist){
			closestDestDist = destDist;
			destNode = &(*iter);
		}
	}

	return vector<Vector3>();
}