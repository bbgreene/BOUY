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
    treeState.addParameterListener("tube", this);
    treeState.addParameterListener("tubeOnOff", this);
    treeState.addParameterListener("multiply", this);
    treeState.addParameterListener("wave", this);
    treeState.addParameterListener("lfo one depth", this);
    treeState.addParameterListener("lfo one rate", this);
    treeState.addParameterListener("lfo two depth", this);
    treeState.addParameterListener("lfo two rate", this);
}

MyTremoloAudioProcessor::~MyTremoloAudioProcessor()
{
    treeState.removeParameterListener("tube", this);
    treeState.removeParameterListener("tubeOnOff", this);
    treeState.removeParameterListener("multiply", this);
    treeState.removeParameterListener("wave", this);
    treeState.removeParameterListener("lfo one depth", this);
    treeState.removeParameterListener("lfo one rate", this);
    treeState.removeParameterListener("lfo two depth", this);
    treeState.removeParameterListener("lfo two rate", this);
}

juce::AudioProcessorValueTreeState::ParameterLayout MyTremoloAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    //LFO One waveform names
    juce::StringArray waveformSelector = {"Sine", "Triangle", "Sloped Square", "Ring"};
    
    auto pTube = std::make_unique<juce::AudioParameterFloat>("tube",
                                                             "Tube",
                                                             juce::NormalisableRange<float>(0.00, 100.0, 0.1, 0.3),
                                                             0.0,
                                                             juce::String(),
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             [](float value, int) {return (value < 100.0f) ? juce::String (value, 1) + " %" : juce::String (value, 0) + " %";});
    
    auto pTubeOnOff = std::make_unique<juce::AudioParameterBool>("tubeOnOff", "TubeOnOff", 0);
    auto pMultiplier = std::make_unique<juce::AudioParameterInt>("multiply", "Multiply", 1, 5, 1);
    auto pWaveform = std::make_unique<juce::AudioParameterChoice>("wave", "Wave", waveformSelector, 0);
    
    auto pDepthOne = std::make_unique<juce::AudioParameterFloat>("lfo one depth",
                                                                 "LFO 1 Depth",
                                                                 juce::NormalisableRange<float>(0.00, 100.0, 0.1, 1.0),
                                                                 0.0,
                                                                 juce::String(),
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 [](float value, int) {return (value < 100.0f) ? juce::String (value, 1) + " %" : juce::String (value, 0) + " %";});
    
    auto pFreqOne = std::make_unique<juce::AudioParameterFloat>("lfo one rate",
                                                                "LFO 1 Rate",
                                                                juce::NormalisableRange<float>(0.01, 100.0, 0.01, 0.4),
                                                                0.01,
                                                                juce::String(),
                                                                juce::AudioProcessorParameter::genericParameter,
                                                                [](float value, int) {return (value < 10.0f) ? juce::String (value, 2) + " Hz" : ((value == 100.0f) ? juce::String (value, 0) + " Hz" : juce::String (value, 1) + " Hz" );});
   
    
    auto pDepthTwo = std::make_unique<juce::AudioParameterFloat>("lfo two depth",
                                                                 "LFO 2 Depth",
                                                                 juce::NormalisableRange<float>(0.00, 100.0, 0.01, 0.5),
                                                                 0.0,
                                                                 juce::String(),
                                                                 juce::AudioProcessorParameter::genericParameter,
                                                                 [](float value, int) {return (value < 100.0f) ? juce::String (value, 1) + " %" : juce::String (value, 0) + " %";});
    
    auto pFreqTwo = std::make_unique<juce::AudioParameterFloat>("lfo two rate",
                                                                "LFO 2 Rate",
                                                                juce::NormalisableRange<float>(0.01, 1.0, 0.01, 1.0),
                                                                0.01,
                                                                juce::String(),
                                                                juce::AudioProcessorParameter::genericParameter,
                                                                [](float value, int) {return (value < 1.0f) ? juce::String (value, 2) + " Hz" : juce::String (value, 1) + " Hz" ;});
    
    params.push_back(std::move(pTube));
    params.push_back(std::move(pTubeOnOff));
    params.push_back(std::move(pMultiplier));
    params.push_back(std::move(pWaveform));
    params.push_back(std::move(pDepthOne));
    params.push_back(std::move(pFreqOne));
    params.push_back(std::move(pDepthTwo));
    params.push_back(std::move(pFreqTwo));
    
    return { params.begin(), params.end() };
}

void MyTremoloAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue)
{
    depthOne.setTargetValue(treeState.getRawParameterValue("lfo one depth")->load());
    freqOne.setTargetValue(treeState.getRawParameterValue("lfo one rate")->load());
    depthTwo.setTargetValue(treeState.getRawParameterValue("lfo two depth")->load());
    freqTwo.setTargetValue(treeState.getRawParameterValue("lfo two rate")->load());
    

    if(parameterID == "wave")
    {
        waveform = newValue;
    }
    if(parameterID == "multiply")
    {
        multiplier = newValue;
    }
    if (parameterID == "tube")
    {
        tubeInPercentage = newValue;
        tubeInGain = juce::jmap(tubeInPercentage, 0.0f, 100.0f, 0.0f, 24.0f); //converting tube percentage to decibels
        rawInput = juce::Decibels::decibelsToGain(tubeInGain);
    }
    if(parameterID == "tubeOnOff")
    {
        tubeOnOff = newValue;
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
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumInputChannels();
    
    depthOne.reset(sampleRate, 0.001);
    freqOne.reset(sampleRate, 0.001);
    depthTwo.reset(sampleRate, 0.001);
    freqTwo.reset(sampleRate, 0.001);
    lfoOnePhase.reset(sampleRate, 0.001);
    lfoTwoPhase.reset(sampleRate, 0.001);
    
    tubeInPercentage = treeState.getRawParameterValue("tube")->load();
    tubeInGain = juce::jmap(tubeInPercentage, 0.0f, 100.0f, 0.0f, 24.0f); //converting tube percentage to decibels
    rawInput = juce::Decibels::decibelsToGain(tubeInGain);
    tubeOnOff = treeState.getRawParameterValue("tubeOnOff")->load();
    depthOne.setCurrentAndTargetValue(treeState.getRawParameterValue("lfo one depth")->load());
    freqOne.setCurrentAndTargetValue(treeState.getRawParameterValue("lfo one rate")->load());
    depthTwo.setCurrentAndTargetValue(treeState.getRawParameterValue("lfo two depth")->load());
    freqTwo.setCurrentAndTargetValue(treeState.getRawParameterValue("lfo two rate")->load());
    
    waveform = treeState.getRawParameterValue("wave")->load();
    multiplier = *treeState.getRawParameterValue("multiply");
        
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
    float myDepthOnePercentage = *treeState.getRawParameterValue("lfo one depth"); //getting 0 - 100 from dial
    float myDepthOne = juce::jmap(myDepthOnePercentage, 0.0f, 100.0f, 0.0f, 1.0f); // converting to 0 - 1
    depthOne.setTargetValue(myDepthOne);
    float myFreqOne = *treeState.getRawParameterValue("lfo one rate");
    freqOne.setTargetValue(myFreqOne);

    float currentDepthOne = depthOne.getNextValue();
    float currentFrequencyOne = freqOne.getNextValue();

    //LFO Two parameters
    float myDepthTwoPercentage = *treeState.getRawParameterValue("lfo two depth"); //getting 0 - 100 from dial
    float myDepthTwo = juce::jmap(myDepthTwoPercentage, 0.0f, 100.0f, 0.0f, 40.0f); // converting to 0 - 40
    depthTwo.setTargetValue(myDepthTwo);
    float myFreqTwo = *treeState.getRawParameterValue("lfo two rate");
    freqTwo.setTargetValue(myFreqTwo);

    float currentDepthTwo = depthTwo.getNextValue();
    float currentFrequencyTwo = freqTwo.getNextValue();

    //Multiplier
    currentFrequencyOne *= multiplier;

    float phaseOne = lfoOnePhase.getNextValue();
    float phaseTwo = lfoTwoPhase.getNextValue();

    //Processing Tremolo
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        phaseOne = lfoOnePhase.getNextValue();
        phaseTwo = lfoTwoPhase.getNextValue();

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                //Distortion - add a bool from a push button
                channelData[sample] = tubeData(channelData[sample]);
                
                const float in = channelData[sample];
                // Tremolo
                float out = in * (1 - currentDepthOne + currentDepthOne * lfoOne(phaseOne, waveform));
                channelData[sample] = out;

                // lfo two output
                float lfoTwoOut = currentDepthTwo * lfoTwo(phaseTwo);

                // Update the carrier and LFO One phases, keeping them in the range 0-1. Here is were LFO two is modulating freq of LFO one
                phaseOne += (currentFrequencyOne + lfoTwoOut) * inverseSampleRate;
                if (phaseOne >= 1.0)
                phaseOne -= 1.0;

                // Update the carrier and LFO Two phases, keeping them in the range 0-1
                phaseTwo += currentFrequencyTwo * inverseSampleRate;
                if (phaseTwo >= 1.0)
                phaseTwo -= 1.0;
            }
    }
    lfoOnePhase = phaseOne;
    lfoTwoPhase = phaseTwo;

}

//==============================================================================
bool MyTremoloAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MyTremoloAudioProcessor::createEditor()
{
    return new MyTremoloAudioProcessorEditor (*this);
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
            // Square with sloped edges
            if(phase < 0.48f)
                return 1.0f;
            else if(phase < 0.5f)
                return 1.0f - 50.0f*(phase - 0.48f);
            else if(phase < 0.98f)
                return 0.0f;
            else
                return 50.0f*(phase - 0.98f);
        case 3:
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

// softclip algorithim (rounded)
float MyTremoloAudioProcessor::tubeData(float samples)
{
    if(tubeOnOff)
    {
        samples *= rawInput * 1.6;
        
        if (samples < 0.0)
        {
            samples = piDivisor * std::atan(samples);
        }
        else if (std::abs(samples) > 1.0)
        {
            // if true then this will output 1 (or -1)
            samples *= 1.0 / std::abs(samples);
        }
        samples = piDivisor * std::atan(samples);
    }
//    samples = piDivisor * std::atan(samples);
    return samples;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MyTremoloAudioProcessor();
}
