/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class Dirty_ol_D_layAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    Dirty_ol_D_layAudioProcessor();
    ~Dirty_ol_D_layAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    int getNumParameters() override;
    float getParameter(int index) override;
    float getViewParameter(int index);
    void setParameter(int index, float newValue) override;


    enum Parameters
    {
        delayLengthParam = 0,
        feedbackParam,
        dirtParam,
        highPassParam,
        mixParam
    };

    float delayLength;
    float feedback;
    float highPass;
    float dirt;
    float mix;

private:
    // circular buffer variables
    juce::AudioSampleBuffer delayBuffer;
    int delayBufferLength;
    int delayReadPosition;
    int delayWritePosition;
    juce::dsp::Oscillator<float> lfo_left;
    juce::dsp::Oscillator<float> lfo_right;
    juce::dsp::IIR::Filter<float> variableHPF_left;
    juce::dsp::IIR::Filter<float> variableHPF_right;
    juce::dsp::IIR::Filter<float> fixedLPF_left;
    juce::dsp::IIR::Filter<float> fixedLPF_right;

    juce::AudioProcessorValueTreeState mState;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Dirty_ol_D_layAudioProcessor)
};
