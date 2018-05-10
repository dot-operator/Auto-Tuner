/*
  ==============================================================================

    CircBuffer.h
    Created: 1 Mar 2018 5:09:48pm
    Author:  vanSulli

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
//#include <memory>
#include <vector>

#define NUM_PITCHSLOTS 888
#define MINIMUM_WAVELENGTH 12

class Resampler
{
public:
	// Copy the frame into the ring buffer
	void Copy(float* channel, unsigned numSamples);

	void SetResampleInfo(float inHz, float outHz = 0.f);
	const inline float GetResampleRate() { return fResampleRate; };
	const inline unsigned GetInputCursor() {
		return uInputCursor;
	};

	const inline unsigned GetBufferSize() {
		return uBuffSize;
	}
	
	// float fOutputCursor{ -5.f };

	// Accept fractional sample positions and interpolate
	float& operator[](float x);
	float& operator[](int x);

	//Resampler& operator=(Resampler other);
	//Resampler(Resampler&& other);
	Resampler(unsigned blocksize = 1);
	~Resampler() {};
private:
	// Need to preserve the type so we have copies of these functions for each
	//inline float ToBufferPosition(float x);
	inline unsigned ToBufferPosition(int x);

	//std::unique_ptr<float[]> buff;
	std::vector<float> buff;

	unsigned uBuffSize, uInputCursor{ 0 };

	// Fractional sample rate output multiplier
	float fResampleRate{ 1.f };
};