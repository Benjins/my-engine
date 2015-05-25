#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>

using std::vector;

template<typename T>
struct KeyFrame{
	T value;
	float time;

	KeyFrame(T _value, float _time){
		value = _value;
		time = _time;
	}
};

template<typename T>
struct Animation{
	//keyFrames is sorted in chronological order
	vector<KeyFrame<T>> keyFrames;

	void AddKeyFrame(T val, float time){
		if(IsEmpty()){
			keyFrames.push_back(KeyFrame<T>(val, time));
		}
		else{
			for(int i = 0; i < keyFrames.size(); i++){
				if(keyFrames[i].time > time){
					keyFrames.insert(keyFrames.begin() + i, KeyFrame<T>(val, time));
					return;
				}
			}

			keyFrames.push_back(KeyFrame<T>(val, time));
		}
	}

	bool IsEmpty() const{
		return keyFrames.size() == 0;
	}

	T Evaluate(float time) const{
		for(int i = 0; i < keyFrames.size(); i++){
			if(keyFrames[i].time > time){
				int postIndex = i;
				int preIndex = i-1;

				if(preIndex == -1){
					return keyFrames[0].value;
				}
				else{
					T preVal  = keyFrames[preIndex].value;
					T postVal = keyFrames[postIndex].value;

					float interpolationLength = keyFrames[postIndex].time - keyFrames[preIndex].time;
					float evalPosition = time - keyFrames[preIndex].time;
					float weight = evalPosition / interpolationLength;

					return preVal * (1 - weight) + postVal * weight;
				}
			}
		}

		if(IsEmpty()){
			cout << "\n\nError: Trying to evaluate an empty animation at time: " << time << "\n";
		}

		return keyFrames[keyFrames.size()-1].value;
	}

	float Length() const{
		int lastIndex = keyFrames.size() - 1;

		if(lastIndex == -1){
			return 0;
		}
		else{
			return keyFrames[lastIndex].time;
		}
	}
	
};

#endif