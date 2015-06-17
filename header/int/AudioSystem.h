#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H

#include <AL/al.h>
#include <AL/alc.h>

#include "../ext/Vector3.h"

#include <vector>

using std::vector;

struct AudioClip{
	int dataSize;

	int id;

	ALuint source;
	ALuint buffer;
	ALuint frequency;
	ALenum format;

	AudioClip(){
		dataSize = 0;
		source = 0;
		buffer = 0;
		format = 0;
	}

	void Play(){
		alSourcePlay(source);
	}

	void SetPosition(const Vector3& pos){
		ALfloat place[] = {pos.x, pos.y, pos.z};
		alListenerfv(AL_POSITION, place);
	}

	void LoadFromWavFile(const char* name);
};

struct AudioSystem{
	ALCdevice* device;
	ALCcontext* context;

	vector<AudioClip> clips;

	AudioSystem(){
		device = nullptr;
		context = nullptr;
	}

	void Initialise();

	void SetListenerPos(const Vector3& pos){
		ALfloat listen[] = {pos.x, pos.y, pos.z};
		alListenerfv(AL_POSITION, listen);
	}

	int AddClip();

	AudioClip* GetClipById(int id);

	~AudioSystem(){
		for(AudioClip& clip : clips){
			alDeleteSources(1, &clip.source);
			alDeleteBuffers(1, &clip.buffer);
		}

		clips.clear();

		alcMakeContextCurrent(nullptr);
		alcDestroyContext(context);
		alcCloseDevice(device);
	}
};



#endif