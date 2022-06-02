/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "../Source/bbg_gui/Dial.h"
#include "../Source/bbg_gui/Toggle.h"
#include "../Source/bbg_gui/PushButton.h"
#include "../Source/bbg_gui/Menu.h"
#include "../Source/bbg_gui/Label.h"

//==============================================================================
/**
*/
class MyTremoloAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    MyTremoloAudioProcessorEditor (MyTremoloAudioProcessor&);
    ~MyTremoloAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    
    //UI dials and Buttons
    bbg_gui::bbg_Dial tube { " %", 0.0, 100.0, 0.01, 0.0, 0.0 };
    bbg_gui::bbg_Dial amountOne { " %", 0.0, 100.0, 0.01, 0.0, 0.0 };
    bbg_gui::bbg_Dial rateOne { " Hz", 0.01, 100.0, 0.01, 0.01, 0.0 };
    bbg_gui::bbg_Dial amountTwo { " %", 0.0, 100.0, 0.01, 0.0, 0.0 };
    bbg_gui::bbg_Dial rateTwo { " Hz", 0.1, 1.0, 0.1, 0.0, 0.0 };
    bbg_gui::bbg_Dial multiplier { " x", 1.0, 5.0, 1.0, 1.0, 0.0 };
    bbg_gui::bbg_Menu waveform;
    bbg_gui::bbg_PushButton tubeOnOff { "On" };
    
    //Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> tubeAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> amountOneAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rateOneAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> amountTwoAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rateTwoAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> multiplierAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> waveformAttachement;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> tubeOnOffAttachement;
    
    //Labels
    bbg_gui::bbg_dialLabel tubeLabel { "Tube" };
    bbg_gui::bbg_dialLabel amountOneLabel { "Amount" };
    bbg_gui::bbg_dialLabel rateOneLabel { "Rate" };
    bbg_gui::bbg_dialLabel amountTwoLabel { "Amount" };
    bbg_gui::bbg_dialLabel rateTwoLabel { "Rate" };
    bbg_gui::bbg_dialLabel multiplierLabel { "Multiply" };
    bbg_gui::bbg_dialLabel lfoOneLabel { "LFO 1" };
    bbg_gui::bbg_dialLabel lfoTwoLabel { "LFO 2" };
    bbg_gui::bbg_dialLabel olumay { "Olumay dsp" };
    bbg_gui::bbg_dialLabel tilTitle { "T I L" };
    bbg_gui::bbg_dialLabel tilVersion { "version 1.1" };
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MyTremoloAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyTremoloAudioProcessorEditor)
};
