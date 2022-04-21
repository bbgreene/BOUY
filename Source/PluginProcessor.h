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
class MyTremoloAudioProcessor  : public juce::AudioProcessor, public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    MyTremoloAudioProcessor();
    ~MyTremoloAudioProcessor() override;

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
    
    juce::AudioProcessorValueTreeState treeState;

private:
    
    //Variables
    juce::LinearSmoothedValue<float> depthOne { 0.0 };
    juce::LinearSmoothedValue<float> freqOne { 0.0 };
    juce::LinearSmoothedValue<float> lfoOnePhase { 0.0 };
    
    juce::LinearSmoothedValue<float> depthTwo { 0.0 };
    juce::LinearSmoothedValue<float> freqTwo { 0.0 };
    juce::LinearSmoothedValue<float> lfoTwoPhase { 0.0 };
    
    //LFO one function for waveform types
    float lfoOne(float phase, int choice);
    
    //LFO two function for waveform types
    float lfoTwo(float phaseTwo);
    
    int waveform  { 0 };
    int multiplier { 1 };
//    bool ringMod { false };
    
    // for LFO phase update
    float inverseSampleRate;
    
    //Parameters
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void parameterChanged (const juce::String& parameterID, float newValue) override;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyTremoloAudioProcessor)
};
