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

	KeyState mouseLeft;
	KeyState mouseMiddle;
	KeyState mouseRight;

	void PushKey(unsigned char key);
	void ReleaseKey(unsigned char key);

	void PushMouse(int button);
	void ReleaseMouse(int button);

	void EndFrame();

public:
	Input();

	bool GetKey(unsigned char key) const;
	bool GetKeyUp(unsigned char key) const;
	bool GetKeyDown(unsigned char key) const;

	bool GetMouse(int button) const;
	bool GetMouseUp(int button) const;
	bool GetMouseDown(int button) const;

};

#endif