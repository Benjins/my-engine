#include <vector>
#include <string>
#include "../ext/Vector2.h"
#include "../ext/Vector3.h"

using std::vector; using std::string;

struct Triangle{
	int v0;
	int v1;
	int v2;
	
	Vector2 uv0;
	Vector2 uv1;
	Vector2 uv2;
	
	Triangle();
	Triangle(int _v0, int _v1, int _v2);
};

struct Vertex{
	Vector3 position;
	Vector3 color;
	//Vector3 uv;
	
	Vertex(void);
	Vertex(Vector3 _position);
};

struct Model{
	vector<Vertex> vertices;
	vector<Triangle> faces;
	
	Model(void);
	Model(const Model& model);
	Model(string fileName);
	
	int GLVertexCount() const;
	
	void ImportFromOBJ(string fileName);
};

Vertex ParseVertexLine(string line);

Triangle ParseFaceLine(string line);

vector<string> SplitStringByDelimiter(string searchIn, string delimiter);