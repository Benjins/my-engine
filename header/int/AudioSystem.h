#include <AL/al.h>
#include <AL/alc.h>

struct AudioClip{
	unsigned char* data;
	int dataSize;

	ALuint source;
	ALuint buffer;
	ALuint frequency;
	ALenum format;

	AudioClip(){
		data = nullptr;
		dataSize = 0;
		source = 0;
		buffer = 0;
		format = 0;
	}

	void Play(){
		alSourcePlay(source);
	}

	void LoadFromWavFile(const char* name);

	~AudioClip(){
		delete[] data;
		alDeleteSources(1, &source);
		alDeleteBuffers(1, &buffer);
	}
};

struct AudioSystem{
	ALCdevice* device;
	ALCcontext* context;

	AudioClip clip;

	AudioSystem(){
		device = nullptr;
		context = nullptr;
	}

	void Initialise();

	~AudioSystem(){
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(context);
		alcCloseDevice(device);
	}
};