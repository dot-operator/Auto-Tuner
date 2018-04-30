/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
AutoTunerAudioProcessor::AutoTunerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

AutoTunerAudioProcessor::~AutoTunerAudioProcessor()
{
}

//==============================================================================
const String AutoTunerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AutoTunerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AutoTunerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AutoTunerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AutoTunerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AutoTunerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AutoTunerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AutoTunerAudioProcessor::setCurrentProgram (int index)
{
}

const String AutoTunerAudioProcessor::getProgramName (int index)
{
    return {};
}

void AutoTunerAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void AutoTunerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
	monobuff = std::unique_ptr<float[]>((float*)malloc(sizeof(float) * samplesPerBlock));
	dSampleRate = sampleRate;
	resampler = Resampler(samplesPerBlock);
	
	FloatVectorOperations::clear(arrAccumEnergy, NUM_PITCHSLOTS);
	FloatVectorOperations::clear(arrAutoCorr, NUM_PITCHSLOTS);
	FloatVectorOperations::clear(arrScratchpad, NUM_PITCHSLOTS);
	uDetectedPeriod = 0;
	uSampleCount = 0;
	iDetectionCursor = 0;
	fOutputCursor = 0.f;
}

void AutoTunerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AutoTunerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void AutoTunerAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

	ProcessMidiMessages(midiMessages);

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

	FloatVectorOperations::clear(monobuff.get(), buffer.getNumSamples());

	// Fill the resampler's buffer.
	auto* channelData = buffer.getWritePointer(0);
	unsigned index = resampler.GetInputCursor();
	resampler.Copy(channelData, buffer.getNumSamples());

	for (size_t i = 0; i < buffer.getNumSamples(); ++i) {
		int iInputCursor = (iDetectionCursor++ % resampler.GetBufferSize()).toInteger();
		UpdateAutocorrelation(iInputCursor);
		if (bPitchStale) {
			if (TryUpdatePitch()) {
				bResample = true;
			}
			else bResample = false;
		}

		// move the output cursor forward by the resample rate.
		monobuff[i] = resampler[fOutputCursor];
		if (bResample && (uKeysDown > 0)) {
			resampler.SetResampleInfo((dSampleRate / (float)uDetectedPeriod), fDesiredPeriod);
			fOutputCursor += resampler.GetResampleRate();
			if (fOutputCursor > (iInputCursor - 5))
				fOutputCursor -= uDetectedPeriod;
			else if (fOutputCursor <= (iInputCursor - uDetectedPeriod))
				fOutputCursor += uDetectedPeriod;
		}
		else ++fOutputCursor;
		fOutputCursor = fmod(fOutputCursor, resampler.GetBufferSize());
	}

    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
		FloatVectorOperations::copy(buffer.getWritePointer(channel), monobuff.get(), buffer.getNumSamples());
    }
}

//==============================================================================
bool AutoTunerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* AutoTunerAudioProcessor::createEditor()
{
    return new AutoTunerAudioProcessorEditor (*this);
}

//==============================================================================
void AutoTunerAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void AutoTunerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

inline void AutoTunerAudioProcessor::UpdateAutocorrelation(int pos)
{
	// Acc energy
	// Vector add most recent sample squared.
	float curSample = resampler[pos];
	FloatVectorOperations::add(arrAccumEnergy, curSample * curSample, NUM_PITCHSLOTS);
	for (size_t i = 0; i < NUM_PITCHSLOTS; ++i) {
		// Remove the oldest relevant sample.
		int window = pos - (12 + i);
		int twowindow = pos - (12 + i) * 2;
		float oneago = resampler[window];
		float twoago = resampler[twowindow];
		arrAccumEnergy[i] -= (twoago * twoago);

		// autocorrelation
		// Add current sample * sample one window length ago.
		arrAutoCorr[i] += (curSample * oneago);
		// Subtract oldest relevant sample.
		arrAutoCorr[i] -= (oneago * twoago);
	}
	if (++uSampleCount >= 5)
		bPitchStale = true;
}

inline bool AutoTunerAudioProcessor::TryUpdatePitch()
{
	// Compute the difference between the accumulated energy and autocorrelation.
	FloatVectorOperations::copy(arrScratchpad, arrAccumEnergy, NUM_PITCHSLOTS);
	FloatVectorOperations::subtractWithMultiply(arrScratchpad, arrAutoCorr, 2.f, NUM_PITCHSLOTS);
	
	// Find the first minimum of these differences.
	// If the autocor. was normalized, this would be the sample closest
	// to an autocorrelation of 1.
	size_t uBestDifferencePos = 0;
	bool bCheckedHarmonic = false;
	for (size_t i = 1; i < NUM_PITCHSLOTS; ++i) {
		if (arrScratchpad[i] < arrScratchpad[i - 1] && arrScratchpad[i + 1] >= arrScratchpad[i]) {
			
			if (uBestDifferencePos == 0 || (uBestDifferencePos != 0 && (arrScratchpad[i] < arrScratchpad[uBestDifferencePos]))) {
				uBestDifferencePos = i;
			}
			// ...or, if the detected frequency is high enough, check again for the first harmonic.
			if (!bCheckedHarmonic && (i < NUM_PITCHSLOTS / 2)) {
				bCheckedHarmonic = true;
				continue;
			}

			uDetectedPeriod = uBestDifferencePos + MINIMUM_WAVELENGTH;
			uSampleCount = 0;
			bPitchStale = false;
			return true;
		}
	}

#if 0
	float bestdifference = FloatVectorOperations::findMinimum(arrScratchpad, NUM_PITCHSLOTS);
	for (size_t i = 0; i < (NUM_PITCHSLOTS / 4); ++i) {
		if (arrScratchpad[i] == bestdifference && (arrScratchpad[i] <= (fDetectionEpsilon * arrAccumEnergy[i]))) {
			uDetectedPeriod = i + MINIMUM_WAVELENGTH;
			uSampleCount = 0;
			bPitchStale = false;
			return true;
		}
	}
#endif
	// Disable the correction if a pitch isn't found.
	uDetectedPeriod = 0;
	resampler.SetResampleInfo(0.f, 0.f);
	return false;
}

void AutoTunerAudioProcessor::ProcessMidiMessages(const MidiBuffer & midi)
{
	// Very simple mono midi note detection.
	int time;
	MidiMessage m;

	for (MidiBuffer::Iterator i(midi); i.getNextEvent(m, time);)
	{
		if (m.isNoteOn()) {
			fDesiredPeriod = (m.getMidiNoteInHertz(m.getNoteNumber()) / 2.0);
			++uKeysDown;
			//bResample = true;
		}
		else if (m.isNoteOff()) {
			if (uKeysDown > 0) {
				--uKeysDown;
				//bResample = false;
			}
		}
	}
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AutoTunerAudioProcessor();
}
