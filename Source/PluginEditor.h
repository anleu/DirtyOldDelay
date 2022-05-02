/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once


#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class Dirty_ol_D_layAudioProcessorEditor  : public juce::AudioProcessorEditor, juce::Slider::Listener
{
public:
    Dirty_ol_D_layAudioProcessorEditor (Dirty_ol_D_layAudioProcessor&);
    ~Dirty_ol_D_layAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Dirty_ol_D_layAudioProcessor& audioProcessor;

    void sliderValueChanged(juce::Slider* slider) override;

    juce::Label delayTimeLabel;
    juce::Slider delayTimeSlider;
    juce::Label feedbackLabel;
    juce::Slider feedbackSlider;
    juce::Label highPassLabel;
    juce::Slider highPassSlider;
    juce::Label dirtLabel;
    juce::Slider dirtSlider;
    juce::Label mixLabel;
    juce::Slider mixSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Dirty_ol_D_layAudioProcessorEditor)
};
