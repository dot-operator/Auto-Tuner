/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
AutoTunerAudioProcessorEditor::AutoTunerAudioProcessorEditor (AutoTunerAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
    // Slider initialization
	levelSlider.setSliderStyle(Slider::SliderStyle::LinearBar);
	levelSlider.setRange(0.f, 0.4f);
	levelSlider.setTextValueSuffix(" Tracking Accuracy");
	levelSlider.setValue(0.1f);
	addAndMakeVisible(levelSlider);

	levelSlider.addListener(this);
	startTimerHz(10); // Automatically repaint 10x a second.
}

AutoTunerAudioProcessorEditor::~AutoTunerAudioProcessorEditor()
{
}

//==============================================================================
void AutoTunerAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid color)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);

	double freq = processor.GetFreq();
	String str = String(freq) + "Hz";
	g.drawFittedText(str, getLocalBounds(), Justification::centred, 1);
}

void AutoTunerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor.
	levelSlider.setBounds(40, 30, getWidth() - 60, 20);
}

void AutoTunerAudioProcessorEditor::sliderValueChanged(Slider * slider)
{
	processor.fDetectionEpsilon = levelSlider.getValue();
}

void AutoTunerAudioProcessorEditor::timerCallback()
{
	repaint();
}
