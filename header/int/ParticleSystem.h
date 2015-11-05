#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <random>
#include "../../3dbasics/Vector3.h"
#include "../../3dbasics/Vector2.h"

#pragma once

struct Particle{
	Vector3 position;
	Vector3 velocity;
	float size;
	float age;
};


struct ParticleSystem{
	Particle* particles;
	int maxParticleCount;
	int particleCount;

	float particlesPerSec;
	Texture* tex;
	float maxLifetime;
	float time;
	float timeSinceParticle;

	float particleScale;

	float gravityFactor;

	int seed;

	default_random_engine randomEngine;

	vector<Vector3> positions;
	vector<Vector2> uvs;

	ParticleSystem(){
		maxParticleCount = 10000;
		particleScale = 1.0f;
		particleCount = 0;
		particles = nullptr;
		gravityFactor = 0.0f;
	}

	void Start(){
		if(particles == nullptr){
			delete[] particles;
		}

		time = 0.0f;
		timeSinceParticle = 0.0f;

		particles = new Particle[maxParticleCount];
		particleCount = 0;
		randomEngine.seed(seed);
	}

	void AddParticle(){
		particles[particleCount].position = Vector3(2,3,1);

		int xi = randomEngine() % 20*1000*1000;
		int yi = randomEngine() % 20*1000*1000;
		int zi = randomEngine() % 20*1000*1000;

		double x = xi - 10*1000*1000;
		double y = yi + 40*1000*1000;
		double z = zi - 10*1000*1000;

		x /= 50*1000*1000;
		y /= 50*1000*1000;
		z /= 50*1000*1000;

		particles[particleCount].velocity = Vector3(x,y,z);
		particles[particleCount].size = particleScale;
		particles[particleCount].age = 0.0f;

		particleCount++;
	}

	void RemoveParticle(int index){
		particles[index] = particles[particleCount-1];
		particleCount--;
	}

	void Update(float deltaTime){
		time += deltaTime;

		for(int i = 0; i < particleCount; i++){
			particles[i].position = particles[i].position + particles[i].velocity*deltaTime;
			particles[i].velocity = particles[i].velocity + Vector3(0,-9.8f,0) * gravityFactor * deltaTime;
			
			particles[i].age += deltaTime;

			if(particles[i].age > maxLifetime){
				RemoveParticle(i);
				i--;
			}
		}

		timeSinceParticle += deltaTime;

		while(timeSinceParticle >= 1/particlesPerSec){
			AddParticle();
			timeSinceParticle -= 1/particlesPerSec;
		}
	}

	void FillGLData(SC_Transform* camera){
		positions.reserve(4*particleCount);
		uvs.reserve(4*particleCount);

		positions.clear();
		uvs.clear();

		Vector3 up = camera->Up();
		Vector3 right = camera->Right();

		for(int i = 0; i < particleCount; i++){
			Vector3 particlePos = particles[i].position;
			
			positions.push_back(particlePos + up*particles[i].size + right*particles[i].size);
			positions.push_back(particlePos + up*particles[i].size - right*particles[i].size);
			positions.push_back(particlePos - up*particles[i].size - right*particles[i].size);
			positions.push_back(particlePos - up*particles[i].size + right*particles[i].size);
			

			uvs.push_back(Vector2(1, 1));
			uvs.push_back(Vector2(1, 0));
			uvs.push_back(Vector2(0, 1));
			uvs.push_back(Vector2(0, 0));
		}
	}

	~ParticleSystem(){
		if(particles != nullptr){
			delete[] particles;
		}
	}
};


#endif