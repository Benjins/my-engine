#ifndef PATHFINDING_H
#define PATHFINDING_H

#include <vector>
#include "../ext/Vector3.h"

using std::vector;

struct Scene;

struct PathNode{
	Vector3 position;
	vector<PathNode*> neighbors;
};

struct Pathfinding{
	vector<PathNode> nodes;
	Scene* scene;

	void HookUpNodes(float maxDistance = 20);

	vector<Vector3> FindPath(Vector3 source, Vector3 destination);
};

#endif