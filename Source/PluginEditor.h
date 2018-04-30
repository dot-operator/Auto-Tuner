/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
*/
class AutoTunerAudioProcessorEditor  : public AudioProcessorEditor, private Slider::Listener, private Timer
{
public:
    AutoTunerAudioProcessorEditor (AutoTunerAudioProcessor&);
    ~AutoTunerAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

	void sliderValueChanged(Slider *slider) override;

private:
	void timerCallback() override;
    Slider levelSlider;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AutoTunerAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoTunerAudioProcessorEditor)
};
