/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define PI 3.14159265358979323846

//==============================================================================
Dirty_ol_D_layAudioProcessor::Dirty_ol_D_layAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )

#endif
	,delayBuffer(2, 1),
	mState(*this, nullptr),
{

	// set default values
	delayLength = 0.2;
	dirt = 0.1;
	mix = 0.2;
	highPass = 100.0f;
	feedback = 0.1;
	delayBufferLength = 20000;

    delayReadPosition = 0;
    delayWritePosition = 0;

    lfo_left.initialise([](float x) { return std::sin(x); }, 128);
    lfo_left.setFrequency(5.0f);

    lfo_right.initialise([](float x) { return std::sin(x); }, 128);
    lfo_right.setFrequency(3.0f);

   mState.state = juce::ValueTree("MySetup");
}

Dirty_ol_D_layAudioProcessor::~Dirty_ol_D_layAudioProcessor()
{
}

//==============================================================================
const juce::String Dirty_ol_D_layAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int Dirty_ol_D_layAudioProcessor::getNumParameters()
{
    return 5;
}

float Dirty_ol_D_layAudioProcessor::getParameter(int index)
{
    switch (index) {
    case delayLengthParam:
        return delayLength;
    case feedbackParam:
        return feedback;
    case dirtParam:
        return dirt;
    case highPassParam:
        return highPass;
    case mixParam:
        return mix;
    default:
        return 0.0f;
    }
}

float Dirty_ol_D_layAudioProcessor::getViewParameter(int index) {
	switch (index) {
	case delayLengthParam:
		return delayLength * 1000;
	case feedbackParam:
		return feedback * 100;
	case dirtParam:
		return dirt * 100;
	case highPassParam:
		return highPass;
	case mixParam:
		return mix * 100;
	default:
		return 0.0f;
	}
}

void Dirty_ol_D_layAudioProcessor::setParameter(int index, float newValue)
{
    switch (index) {
    case feedbackParam:
        feedback = newValue / 100.0;
        break;
    case dirtParam:
        dirt = newValue / 100.0;
        break;
    case highPassParam:
        highPass = newValue;
        variableHPF_left.reset();
        variableHPF_left.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(getSampleRate(), highPass);
        variableHPF_right.reset();
        variableHPF_right.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(getSampleRate(), highPass);
        break;
    case delayLengthParam:
        delayLength = newValue / 1000.0;
        delayReadPosition = (int)(delayWritePosition - (delayLength * getSampleRate()) + delayBufferLength) % delayBufferLength;
        break;
    case mixParam:

        mix = newValue / 100.0;
    default:
        break;;
    }
}

bool Dirty_ol_D_layAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Dirty_ol_D_layAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Dirty_ol_D_layAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Dirty_ol_D_layAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Dirty_ol_D_layAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Dirty_ol_D_layAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Dirty_ol_D_layAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Dirty_ol_D_layAudioProcessor::getProgramName (int index)
{
    return {};
}

void Dirty_ol_D_layAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Dirty_ol_D_layAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    delayBufferLength = (int) sampleRate;
    if (delayBufferLength < 1)
        delayBufferLength = 1;

    delayBuffer.setSize(2, delayBufferLength);
    delayBuffer.clear();

    delayReadPosition = (int)(delayWritePosition - (delayLength * getSampleRate()) + delayBufferLength) % delayBufferLength;

    fixedLPF_left.reset();
    fixedLPF_left.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), 5000.00);
    fixedLPF_right.reset();
    fixedLPF_right.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), 5000.00);
    
    juce::dsp::ProcessSpec spec{ sampleRate / 100, static_cast<juce::uint32> (samplesPerBlock) };
    lfo_left.prepare(spec);
    lfo_right.prepare(spec);
}

void Dirty_ol_D_layAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Dirty_ol_D_layAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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

float addGain(float gain, float sample) {
	return powf(10.0f, gain * 0.05f) * sample;
}

float reduceGain(float gain, float sample) {
	return sample / powf(10.0f, gain * 0.05f);
}

float softClip(float sample, float dirt) {

	float gain = 30.0f;

	float sampleWithGain = addGain(gain, sample);

	float out;
	float threshold1 = 1.0f / 3.0f;
	float threshold2 = 2.0f / 3.0f;
	if (sampleWithGain > threshold2)
		out = 1.0f;
	else if (sampleWithGain > threshold1)
		out = 1.0f - powf(2.0f - 3.0f * sampleWithGain, 2.0f) / 3.0f;
	else if (sampleWithGain < -threshold2)
		out = -1.0f;
	else if (sampleWithGain < -threshold1)
		out = -1.0f + powf(2.0f + 3.0f * sampleWithGain, 2.0f) / 3.0f;
	else
		out = 2.0f * sampleWithGain;
	out *= 0.5f;

	return reduceGain(gain, out);
}

float addDirt(float sample, float dirt) {
    return (1 - dirt) * sample + dirt * softClip(sample, dirt);
}

void Dirty_ol_D_layAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    const int numInputChannels = getNumInputChannels();
    const int numOutputChannels = getNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    int dpr, dpw;

    for (int channel = 0; channel < numInputChannels; ++channel) {
        float* channelData = buffer.getWritePointer(channel);
        float* delayData = delayBuffer.getWritePointer(juce::jmin(channel, delayBuffer.getNumChannels() - 1));

        juce::dsp::Oscillator<float>* lfo;
        juce::dsp::IIR::Filter<float>* variableHPF;
        juce::dsp::IIR::Filter<float>* fixedLPF;

        if (channel == 0) {
            lfo = &lfo_left;
            variableHPF = &variableHPF_left;
            fixedLPF = &fixedLPF_left;
        }
        else {
            lfo = &lfo_right;
            variableHPF = &variableHPF_right;
            fixedLPF = &fixedLPF_right;
        }

		dpr = delayReadPosition;
		dpw = delayWritePosition;

		for (int i = 0; i < numSamples; ++i) {

			const float in = channelData[i];
            auto lfoOut = lfo->processSample(0.0f);
            float modulation = juce::jmap(lfoOut, -1.0f, 1.0f, 0.85f, 0.95f);
            int modulatedReadPoint = floorl(dpr * modulation);
            if(modulatedReadPoint >= delayBufferLength)
            {
                modulatedReadPoint = delayBufferLength;
            }


			float out = ((1 - mix) * in + addDirt(mix * delayData[modulatedReadPoint], dirt));
                        	
           
			float delaySample = (delayData[dpr] * feedback)* modulation;
			float newDelayValue = in + delaySample; 

			delayData[dpw] = fixedLPF -> processSample(variableHPF -> processSample((newDelayValue)));

			if (++dpr >= delayBufferLength) 
				dpr = 0;  
			if (++dpw >= delayBufferLength)
				dpw = 0;


			channelData[i] = out;

		}


	}

   

	delayReadPosition = dpr;
    delayWritePosition = dpw;

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // I've added this to avoid people getting screaming feedback
    // when they first compile the plugin, but obviously you don't need to
    // this code if your algorithm already fills all the output channels.
    for (int i = numInputChannels; i < numOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());
}



//==============================================================================
bool Dirty_ol_D_layAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Dirty_ol_D_layAudioProcessor::createEditor()
{
    return new Dirty_ol_D_layAudioProcessorEditor (*this);
}

//==============================================================================
void Dirty_ol_D_layAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    auto s = mState.copyState();
    std::unique_ptr<juce::XmlElement> xml(s.createXml());
    xml->setAttribute("delayLength", delayLength);
    xml->setAttribute("feedback", feedback);
    xml->setAttribute("highpass", highPass);
    xml->setAttribute("dirt", dirt);
    xml->setAttribute("mix", mix);
    copyXmlToBinary(*xml, destData);

}

void Dirty_ol_D_layAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr) {
        if (xmlState->hasTagName(mState.state.getType())) {
            delayLength = xmlState->getDoubleAttribute("delayLength");
            feedback = xmlState->getDoubleAttribute("feedback");
            highPass = xmlState->getDoubleAttribute("highpass");
            dirt = xmlState->getDoubleAttribute("dirt");
            mix = xmlState->getDoubleAttribute("mix");
            mState.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Dirty_ol_D_layAudioProcessor();
}
