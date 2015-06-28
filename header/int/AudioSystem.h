#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H

#if defined(_WIN32) || defined(_WIN64)
#include <al.h>
#include <alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

#include "../ext/Vector3.h"
#include <vector>
#include <string>

#define MAX_CLIP_COUNT 20

using std::vector;

struct AudioClip{
	string clipFileName;
	string clipName;

	ALuint source;
	ALuint buffer;
	ALuint frequency;
	ALenum format;

	void LoadFromWavFile(const char* name);
};

struct AudioSystem{
	ALCdevice* device;
	ALCcontext* context;

	AudioClip clips[MAX_CLIP_COUNT];

	int clipCount;

	AudioSystem(){
		device = nullptr;
		context = nullptr;
		clipCount = 0;
	}

	void Initialise();

	void SetListenerPos(const Vector3& pos){
		ALfloat listen[] = {pos.x, pos.y, pos.z};
		alListenerfv(AL_POSITION, listen);
	}

	AudioClip* AddClip(){
		AudioClip* clip = &clips[clipCount];
		clipCount++;
		return clip;
	}

	AudioClip* FindClip(const string& clipName){
		for(int i = 0; i < clipCount; i++){
			if(clips[i].clipName == clipName){
				return &clips[i];
			}
		}

		return nullptr;
	}

	~AudioSystem(){
		for(int i = 0; i < clipCount; i++){
			alDeleteBuffers(1, &(clips[i].buffer));
		}

		alcMakeContextCurrent(nullptr);
		alcDestroyContext(context);
		alcCloseDevice(device);
	}
};



#endif