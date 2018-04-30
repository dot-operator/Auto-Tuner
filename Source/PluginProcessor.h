/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "Resampler.h"

#define NUM_PITCHSLOTS 888
#define MINIMUM_WAVELENGTH 12

//==============================================================================
/**
*/
class AutoTunerAudioProcessor  : public AudioProcessor
{
public:
	float fDetectionEpsilon{ 0.1f };
    //==============================================================================
    AutoTunerAudioProcessor();
    ~AutoTunerAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
	std::unique_ptr<float[]> monobuff;
	Resampler resampler;
	Random random;
	double dSampleRate;

	// Detector.
	float arrAccumEnergy[NUM_PITCHSLOTS];
	float arrAutoCorr[NUM_PITCHSLOTS];
	float arrScratchpad[NUM_PITCHSLOTS];
	unsigned uDetectedPeriod{ 0 };
	unsigned uSampleCount;
	bool bPitchStale{ true }; // Whether to try to detect a pitch this frame
	BigInteger iDetectionCursor{ 0 };

	// Resampler.
	float fOutputCursor{ -5.f };

	inline void UpdateAutocorrelation(int pos);
	inline bool TryUpdatePitch();
	void ProcessMidiMessages(const MidiBuffer& midi);
	float fDesiredPeriod{ 880.f };
	bool bResample{ false };
	unsigned uKeysDown;

public:
	const inline unsigned GetPeriod() {
		return uDetectedPeriod;
	};

	const inline double GetFreq() {
		return (dSampleRate / (double)uDetectedPeriod);
	}

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoTunerAudioProcessor)
};
