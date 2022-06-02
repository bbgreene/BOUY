/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MyTremoloAudioProcessorEditor::MyTremoloAudioProcessorEditor (MyTremoloAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // set default font
    juce::LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypefaceName ("Avenir Next");
    
    //dial, buttons with their respective attachments
    tubeAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "tube", tube);
    tube.setDialStyle(bbg_gui::bbg_Dial::DialStyle::kDialDotModernStyle);
    addAndMakeVisible(tube);
    
    tubeOnOffAttachement = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.treeState, "tubeOnOff", tubeOnOff);
    addAndMakeVisible(tubeOnOff);
    
    amountOneAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "lfo one depth", amountOne);
    amountOne.setDialStyle(bbg_gui::bbg_Dial::DialStyle::kDialDotModernStyle);
    addAndMakeVisible(amountOne);
    
    rateOneAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "lfo one rate", rateOne);
    rateOne.setDialStyle(bbg_gui::bbg_Dial::DialStyle::kDialDotModernStyle);
    addAndMakeVisible(rateOne);
    
    amountTwoAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "lfo two depth", amountTwo);
    amountTwo.setDialStyle(bbg_gui::bbg_Dial::DialStyle::kDialDotModernStyle);
    addAndMakeVisible(amountTwo);
    
    rateTwoAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "lfo two rate", rateTwo);
    rateTwo.setDialStyle(bbg_gui::bbg_Dial::DialStyle::kDialDotModernStyle);
    addAndMakeVisible(rateTwo);
    
    multiplierAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "multiply", multiplier);
    multiplier.setDialStyle(bbg_gui::bbg_Dial::DialStyle::kDialDotModernStyle);
    addAndMakeVisible(multiplier);
    
    waveformAttachement = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.treeState, "wave", waveform);
    waveform.setText("Sine");
    waveform.addItem("Sine", 1);
    waveform.addItem("Tri", 2);
    waveform.addItem("Square", 3);
    waveform.addItem("Ring", 4);
    addAndMakeVisible(waveform);
    
    //dial label attachments
    tubeLabel.attachToComponent(&tube, false);
    amountOneLabel.attachToComponent(&amountOne, false);
    rateOneLabel.attachToComponent(&rateOne, false);
    amountTwoLabel.attachToComponent(&amountTwo, false);
    rateTwoLabel.attachToComponent(&rateTwo, false);
    multiplierLabel.attachToComponent(&multiplier, false);
    
    
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (300, 500);
}

MyTremoloAudioProcessorEditor::~MyTremoloAudioProcessorEditor()
{
}

//==============================================================================
void MyTremoloAudioProcessorEditor::paint (juce::Graphics& g)
{
    juce::Rectangle<int> background = getLocalBounds();
    g.setGradientFill(juce::ColourGradient::vertical(juce::Colours::lightseagreen.brighter(0.5), getHeight() - getHeight(), juce::Colours::darkorange.brighter(0.5), getHeight()));
    g.fillRect(background);
}

void MyTremoloAudioProcessorEditor::resized()
{
    auto topMargain = getHeight() / 7.41;
    auto dialSize = getHeight() / 3.85;
    auto dialYGap = getHeight() / 18.18;
    auto leftMargin = (getWidth() - (dialSize * 2)) / 2;
    
    auto multiDialSize = dialSize * 0.6;
    auto multiXGap = (dialSize - multiDialSize) / 2;
    auto mutliYGap = getHeight() * 0.1332;
    
    auto waveformWidth = dialSize * 0.6;
    auto waveformHeight = dialSize * 0.2;
    auto waveformXGap = (dialSize - waveformWidth) / 2;
    
    auto tubeOnWidth = dialSize * 0.25;
    auto tubeOnHeight = dialSize * 0.15;
    auto tubeOnXGap = (dialSize - tubeOnWidth) / 2;
    auto tubeOnYGap = getHeight() / 20;
    
    tube.setBounds(leftMargin, topMargain, dialSize, dialSize);
    tubeOnOff.setBounds(leftMargin + tubeOnXGap, tube.getBottom() - tubeOnYGap, tubeOnWidth, tubeOnHeight);
    multiplier.setBounds(tube.getRight() + multiXGap, mutliYGap, multiDialSize, multiDialSize);
    waveform.setBounds(tube.getRight() + waveformXGap, multiplier.getBottom(), waveformWidth, waveformHeight);
    amountOne.setBounds(leftMargin, tube.getBottom() + dialYGap, dialSize, dialSize);
    amountTwo.setBounds(leftMargin, amountOne.getBottom() + dialYGap, dialSize, dialSize);
    rateOne.setBounds(amountOne.getRight(), tube.getBottom() + dialYGap, dialSize, dialSize);
    rateTwo.setBounds(amountTwo.getRight(), rateOne.getBottom() + dialYGap, dialSize, dialSize);
    
}
