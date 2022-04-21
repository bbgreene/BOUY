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
    treeState.addParameterListener("multiplier", this);
    treeState.addParameterListener("depth two", this);
    treeState.addParameterListener("freq two", this);
}

MyTremoloAudioProcessor::~MyTremoloAudioProcessor()
{
    treeState.removeParameterListener("depth", this);
    treeState.removeParameterListener("freq", this);
    treeState.removeParameterListener("wave", this);
    treeState.removeParameterListener("multiplier", this);
    treeState.removeParameterListener("depth two", this);
    treeState.removeParameterListener("freq two", this);
}

juce::AudioProcessorValueTreeState::ParameterLayout MyTremoloAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    //LFO One waveform names
    juce::StringArray waveformSelector = {"Sine", "Triangle", "Square", "Sloped Square", "Ring"};
    
    auto pDepth = std::make_unique<juce::AudioParameterFloat>("depth", "Depth", 0.0, 1.0, 0.5);
    auto pFreq = std::make_unique<juce::AudioParameterFloat>("freq", "Freq", juce::NormalisableRange<float>(0.0, 100.0, 0.01, 0.3), 5.0);
    auto pWaveform = std::make_unique<juce::AudioParameterChoice>("wave", "Wave", waveformSelector, 0);
    auto pMultiplier = std::make_unique<juce::AudioParameterInt>("multiplier", "Multiplier", 0, 4, 1);
    auto pDepthTwo = std::make_unique<juce::AudioParameterFloat>("depth two", "Depth Two", 0.0, 100.0, 0.5);
    auto pFreqTwo = std::make_unique<juce::AudioParameterFloat>("freq two", "Freq Two", juce::NormalisableRange<float>(0.0, 1.0, 0.01, 0.3), 5.0);
    
    params.push_back(std::move(pDepth));
    params.push_back(std::move(pFreq));
    params.push_back(std::move(pWaveform));
    params.push_back(std::move(pMultiplier));
    params.push_back(std::move(pDepthTwo));
    params.push_back(std::move(pFreqTwo));
    
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
    if(parameterID == "multiplier")
    {
        multiplier = newValue;
    }
    if(parameterID == "depth two")
    {
        depthTwo = newValue;
    }
    if(parameterID == "freq two")
    {
        freqTwo = newValue;
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
    depthTwo.reset(sampleRate, 0.0005);
    freqTwo.reset(sampleRate, 0.0005);
    lfoOnePhase.reset(sampleRate, 0.0005);
    lfoTwoPhase.reset(sampleRate, 0.0005);
    
    waveform = treeState.getRawParameterValue("wave")->load();
    multiplier = *treeState.getRawParameterValue("multiplier");
        
    lfoOnePhase = 0.0;
    lfoTwoPhase = 0.0;
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
    
    //LFO One parameters
    float myDepth = *treeState.getRawParameterValue("depth");
    depth.setTargetValue(myDepth);
    float myFreq = *treeState.getRawParameterValue("freq");
    freq.setTargetValue(myFreq);
    
    float currentDepth = depth.getNextValue();
    float currentFrequency = freq.getNextValue();
    
    //LFO Two parameters
    float myDepthTwo = *treeState.getRawParameterValue("depth two");
    depthTwo.setTargetValue(myDepthTwo);
    float myFreqTwo = *treeState.getRawParameterValue("freq two");
    freqTwo.setTargetValue(myFreqTwo);
    
    float currentDepthTwo = depthTwo.getNextValue();
    float currentFrequencyTwo = freqTwo.getNextValue();
        
    //Multiplier
    if(multiplier == 0)
    {
        currentFrequency *= 0.5; // this makes '0' half the freq of LFO - need to use a somehting else for this!
    }
    else
    {
        currentFrequency *= multiplier;
    }
    
    float phase = lfoOnePhase.getNextValue();
    float phaseTwo = lfoTwoPhase.getNextValue();
    
    //Processing Tremolo
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        phase = lfoOnePhase.getNextValue();
        phaseTwo = lfoTwoPhase.getNextValue();

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                const float in = channelData[sample];
                // Tremolo
//                float out = in * (1 - currentDepth + currentDepth * lfoOne(phase, waveform));
//                channelData[sample] = out;
//
                //LFO Two test with LFO one
                float out = in * (1 - currentDepth + currentDepth * lfoOne(phase, waveform));
                channelData[sample] = out;
                
                // lfo two output
                float lfoTwoOut = currentDepthTwo * lfoTwo(phaseTwo);

                // Update the carrier and LFO One phases, keeping them in the range 0-1. Here is were LFO two is modulating freq of LFO one
                phase += (currentFrequency + lfoTwoOut) * inverseSampleRate;
                if (phase >= 1.0)
                phase -= 1.0;
                
                // Update the carrier and LFO Two phases, keeping them in the range 0-1
                phaseTwo += currentFrequencyTwo * inverseSampleRate;
                if (phaseTwo >= 1.0)
                phaseTwo -= 1.0;
            }
    }
    lfoOnePhase = phase;
    lfoTwoPhase = phaseTwo;
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
    // Save params
    juce::MemoryOutputStream stream(destData, false);
    treeState.state.writeToStream(stream);
}

void MyTremoloAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Recall params
    auto tree = juce::ValueTree::readFromData(data, size_t(sizeInBytes));
        
    if(tree.isValid())
    {
        treeState.replaceState(tree);
    }
}

//LFO one (main tremolo LFO) waveform selection
float MyTremoloAudioProcessor::lfoOne(float phase, int choice)
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
        case 4:
            // ring mod
            return sinf(2.0 * M_PI * phase);
            break;
            
        default:
            return 0.5f + 0.5f * sinf(2.0 * M_PI * phase);
            break;
    }
}

//LFO two (modulating LFO one) waveform selection
float MyTremoloAudioProcessor::lfoTwo(float phaseTwo)
{
    return 0.5f + 0.5f * sinf(2.0 * M_PI * phaseTwo);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MyTremoloAudioProcessor();
}
