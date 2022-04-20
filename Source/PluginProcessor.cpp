/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MyTremoloAudioProcessor::MyTremoloAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), treeState(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
    treeState.addParameterListener("depth", this);
    treeState.addParameterListener("freq", this);
    treeState.addParameterListener("wave", this);
}

MyTremoloAudioProcessor::~MyTremoloAudioProcessor()
{
    treeState.removeParameterListener("depth", this);
    treeState.removeParameterListener("freq", this);
    treeState.removeParameterListener("wave", this);
}

juce::AudioProcessorValueTreeState::ParameterLayout MyTremoloAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    juce::StringArray waveformSelector = {"Sine", "Triangle", "Square", "Sloped Square"};
    
    auto pDepth = std::make_unique<juce::AudioParameterFloat>("depth", "Depth", 0.0, 1.0, 0.5);
    auto pFreq = std::make_unique<juce::AudioParameterFloat>("freq", "Freq", juce::NormalisableRange<float>(0.0, 100.0, 0.01, 0.3), 5.0);
    auto pWaveform = std::make_unique<juce::AudioParameterChoice>("wave", "Wave", waveformSelector, 0);
    
    params.push_back(std::move(pDepth));
    params.push_back(std::move(pFreq));
    params.push_back(std::move(pWaveform));
    
    return { params.begin(), params.end() };
}

void MyTremoloAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue)
{
    if(parameterID == "depth")
    {
        depth = newValue;
    }
    if(parameterID == "freq")
    {
        freq = newValue;
    }
    if(parameterID == "wave")
    {
        waveform = newValue;
    }
    
}
//==============================================================================
const juce::String MyTremoloAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MyTremoloAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MyTremoloAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MyTremoloAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MyTremoloAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MyTremoloAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MyTremoloAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MyTremoloAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MyTremoloAudioProcessor::getProgramName (int index)
{
    return {};
}

void MyTremoloAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MyTremoloAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    depth.reset(sampleRate, 0.0005);
    freq.reset(sampleRate, 0.0005);
    lfoPhase.reset(sampleRate, 0.0005);
    
    lfoPhase = 0.0;
    inverseSampleRate = 1.0 / sampleRate;
}

void MyTremoloAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MyTremoloAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void MyTremoloAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    float myDepth = *treeState.getRawParameterValue("depth");
    depth.setTargetValue(myDepth);
    float myFreq = *treeState.getRawParameterValue("freq");
    freq.setTargetValue(myFreq);
    
    float currentDepth = depth.getNextValue();
    float currentFrequency = freq.getNextValue();
    float phase = lfoPhase.getNextValue();
        
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        phase = lfoPhase.getNextValue();
    
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                const float in = channelData[sample];
                float out = in * (1 - currentDepth + currentDepth * lfo(phase, waveform));

                channelData[sample] = out;
                
                phase += currentFrequency * inverseSampleRate;
                if (phase >= 1.0)
                phase -= 1.0;
            }
    }
    lfoPhase = phase;
}

//==============================================================================
bool MyTremoloAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MyTremoloAudioProcessor::createEditor()
{
//    return new MyTremoloAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void MyTremoloAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MyTremoloAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

float MyTremoloAudioProcessor::lfo(float phase, int choice)
{
    switch (choice) {
        case 0:
            // sine wave
            return 0.5 + 0.5 * sinf(2.0 * M_PI * phase);
            break;
        case 1:
            // Triangle
            if(phase < 0.25f)
                return 0.5f + 2.0f*phase;
            else if(phase < 0.75f)
                return 1.0f - 2.0f*(phase - 0.25f);
            else
                return 2.0f*(phase-0.75f);
            break;
        case 2:
            // Square
            if(phase < 0.5f)
            return 1.0f;
            else
            return 0.0f;
            break;
        case 3:
            // Square with sloped edges
            if(phase < 0.48f)
                return 1.0f;
            else if(phase < 0.5f)
                return 1.0f - 50.0f*(phase - 0.48f);
            else if(phase < 0.98f)
                return 0.0f;
            else
                return 50.0f*(phase - 0.98f);
            
        default:
            return 0.5f + 0.5f*sinf(2.0 * M_PI * phase);
            break;
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MyTremoloAudioProcessor();
}
