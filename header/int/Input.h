#ifndef INPUT_H
#define INPUT_H

enum KeyState{
	PRESS,
	RELEASE,
	HOLD,
	OFF
};

struct Input{
	friend struct Scene;

protected:
	KeyState keys[255];

	void PushKey(unsigned char key);
	void ReleaseKey(unsigned char key);

public:
	Input();

	bool GetKey(unsigned char key) const;
	bool GetKeyUp(unsigned char key) const;
	bool GetKeyDown(unsigned char key) const;

	void EndFrame();


};

#endif