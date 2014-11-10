#include "../header/int/Input.h"


Input::Input(){
	for(int i = 0; i < 256; i++){
		keys[i] = KeyState::OFF;
	}
}

void Input::PushKey(unsigned char key){
	keys[key] = KeyState::PRESS;
}

void Input::ReleaseKey(unsigned char key){
	keys[key] = KeyState::RELEASE;
}


bool Input::GetKey(unsigned char key) const{
	return keys[key] == KeyState::PRESS || keys[key] == KeyState::HOLD;
}

bool Input::GetKeyUp(unsigned char key) const{
	return keys[key] == KeyState::RELEASE;
}

bool Input::GetKeyDown(unsigned char key) const{
	return keys[key] == KeyState::PRESS;
}

void Input::EndFrame(){
	for(int i = 0; i < 256; i++){
		if(keys[i] == KeyState::PRESS){
			keys[i] = KeyState::HOLD;
		}
		if(keys[i] == KeyState::RELEASE){
			keys[i] = KeyState::OFF;
		}
	}
}
