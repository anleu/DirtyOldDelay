/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Dirty_ol_D_layAudioProcessorEditor::Dirty_ol_D_layAudioProcessorEditor (Dirty_ol_D_layAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    delayTimeLabel("Delay Time", "Delay Time"),
    feedbackLabel("Feedback", "Feedback"),
    dirtLabel("Dirt", "Dirt"),
    highPassLabel("High Pass", "Low Pass"),
    mixLabel("Mix", "Mix")
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (560, 300);

    delayTimeSlider.setSliderStyle(juce::Slider::Rotary);
    delayTimeSlider.setRange(0.0, 1000, 1.0);
    delayTimeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 40);
    delayTimeSlider.setPopupDisplayEnabled(true, false, this);
    delayTimeSlider.setTextValueSuffix(" ms");
    delayTimeSlider.setValue(audioProcessor.getViewParameter(audioProcessor.delayLengthParam));
    delayTimeSlider.addListener(this);

    addAndMakeVisible(&delayTimeSlider);
    delayTimeLabel.setJustificationType(juce::Justification::centred);
    delayTimeLabel.attachToComponent(&delayTimeSlider, false);

    feedbackSlider.setSliderStyle(juce::Slider::Rotary);
    feedbackSlider.setRange(0.0, 100, 1.0);
    feedbackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 40);
    feedbackSlider.setPopupDisplayEnabled(true, false, this);
    feedbackSlider.setTextValueSuffix(" %");
    feedbackSlider.setValue(audioProcessor.getViewParameter(audioProcessor.feedbackParam));
    feedbackSlider.addListener(this);

    addAndMakeVisible(&feedbackSlider);
    feedbackLabel.setJustificationType(juce::Justification::centred);
    feedbackLabel.attachToComponent(&feedbackSlider, false);

    highPassSlider.setSliderStyle(juce::Slider::Rotary);
    highPassSlider.setRange(20.0, 2000, 1.0);
    highPassSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 40);
    highPassSlider.setPopupDisplayEnabled(true, false, this);
    highPassSlider.setTextValueSuffix(" Hz");
    highPassSlider.setValue(audioProcessor.getViewParameter(audioProcessor.highPassParam));
    highPassSlider.addListener(this);

    addAndMakeVisible(&highPassSlider);
    highPassLabel.setJustificationType(juce::Justification::centred);
    highPassLabel.attachToComponent(&highPassSlider, false);

    dirtSlider.setSliderStyle(juce::Slider::Rotary);
    dirtSlider.setRange(0.0, 100, 1.0);
    dirtSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 40);
    dirtSlider.setPopupDisplayEnabled(true, false, this);
    dirtSlider.setTextValueSuffix("");
    dirtSlider.setValue(audioProcessor.getViewParameter(audioProcessor.dirtParam));
    dirtSlider.addListener(this);

    addAndMakeVisible(&dirtSlider);
    dirtLabel.setJustificationType(juce::Justification::centred);
    dirtLabel.attachToComponent(&dirtSlider, false);

    mixSlider.setSliderStyle(juce::Slider::Rotary);
    mixSlider.setRange(0.0, 100, 1.0);
    mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 40);
    mixSlider.setPopupDisplayEnabled(true, false, this);
    mixSlider.setTextValueSuffix(" %");
    mixSlider.setValue(audioProcessor.getViewParameter(audioProcessor.mixParam));
    mixSlider.addListener(this);

    addAndMakeVisible(&mixSlider);
    mixLabel.setJustificationType(juce::Justification::centred);
    mixLabel.attachToComponent(&mixSlider, false);
}


Dirty_ol_D_layAudioProcessorEditor::~Dirty_ol_D_layAudioProcessorEditor()
{

}

//==============================================================================
void Dirty_ol_D_layAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
   // g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (18.0f);
    g.drawFittedText ("Dirty Old Delay", getLocalBounds(), juce::Justification::centredTop, 1);

    g.setFont(12.0f);
    g.drawFittedText("by AndyExploresHomerecording", getLocalBounds(), juce::Justification::bottomRight, 1);

}

void Dirty_ol_D_layAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    auto area = getLocalBounds();

    delayTimeSlider.setBounds(10,60,100,150);
    feedbackSlider.setBounds(120,60,100,150);
    highPassSlider.setBounds(230,60,100,150);
    dirtSlider.setBounds(340,60,100,150);
    mixSlider.setBounds(450,60,100,150);
    
    //delayTimeSlider.setBounds(40, 30, 20, getHeight() - 60);
}


void Dirty_ol_D_layAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{

	if (slider == &delayTimeSlider) {
		getAudioProcessor()->setParameter(Dirty_ol_D_layAudioProcessor::delayLengthParam, (float)delayTimeSlider.getValue());
	}
	else if (slider == &feedbackSlider) {
		getAudioProcessor()->setParameter(Dirty_ol_D_layAudioProcessor::feedbackParam, (float)feedbackSlider.getValue());
	}
	else if (slider == &highPassSlider) {
		getAudioProcessor()->setParameter(Dirty_ol_D_layAudioProcessor::highPassParam, (float)highPassSlider.getValue());
	}
	else if (slider == &dirtSlider) {
		getAudioProcessor()->setParameter(Dirty_ol_D_layAudioProcessor::dirtParam, (float)dirtSlider.getValue());
	}
	else if (slider == &mixSlider) {
		getAudioProcessor()->setParameter(Dirty_ol_D_layAudioProcessor::mixParam, (float)mixSlider.getValue());
	}
}


