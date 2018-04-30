/*
  ==============================================================================

    CircBuffer.cpp
    Created: 1 Mar 2018 5:09:48pm
    Author:  vanSulli

  ==============================================================================
*/

#include "Resampler.h"

void Resampler::Copy(float * channel, unsigned numSamples)
{
	// Since we initialized our buffer to be 2x the frame size,
	// this should be a matter of placing it where the cursor left off,
	// without the need to split it.
	// numSamples is just a sanity check.
	if (uBuffSize / 2 != numSamples) {
		std::length_error("Expected the number of samples to be 0.5x the ring buffer.");
		return;
	}

	FloatVectorOperations::copy(&buff[uInputCursor], channel, numSamples);
	uInputCursor = (uInputCursor + numSamples) % uBuffSize;
}

void Resampler::SetResampleInfo(float inHz, float outHz)
{
	if (inHz == 0.f || outHz == 0.f) {
		fResampleRate = 1.f;
	}
	else fResampleRate = outHz / inHz;
}

float & Resampler::operator[](float x)
{
	// just nearest sample right now
	return buff[(int)ToBufferPosition(x)];
}

float & Resampler::operator[](int x)
{
	return buff[ToBufferPosition(x)];
}

#if 0
Resampler & Resampler::operator=(Resampler other)
{
	if (other.buff != nullptr)
		buff = std::move(other.buff);
	uBuffSize = other.uBuffSize;
	uInputCursor = other.uInputCursor;
	fOutputCursor = other.fOutputCursor;
	fResampleRate = other.fResampleRate;
	fInputPeriod = other.fInputPeriod;
	return *this;
}

Resampler::Resampler(Resampler && other)
{
	if(other.buff != nullptr)
		buff = std::move(other.buff);
	uBuffSize = other.uBuffSize;
	uInputCursor = other.uInputCursor;
	fOutputCursor = other.fOutputCursor;
	fResampleRate = other.fResampleRate;
	fInputPeriod = other.fInputPeriod;
}
#endif

Resampler::Resampler(unsigned blocksize)
{
	uBuffSize = blocksize * 2;
	buff.clear();
	buff.insert(buff.end(), uBuffSize, 0.f);
	//buff = std::unique_ptr<float[]>((float*)malloc(sizeof(float) * uBuffSize));
	FloatVectorOperations::clear(&buff[0], blocksize);
}

inline float Resampler::ToBufferPosition(float x)
{
	if (x >= 0) {
		return fmod(x, uBuffSize);
	}
	else {
		// If it's negative, first account for multiples of ring length
		x = fmod(x, uBuffSize);
		// and then count backwards from the 0 position
		return (uBuffSize - x);
	}
}

inline unsigned Resampler::ToBufferPosition(int x)
{
	return x % uBuffSize;
}
