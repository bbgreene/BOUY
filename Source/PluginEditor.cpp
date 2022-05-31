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
    
    amountOneAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "depth one", amountOne);
    amountOne.setDialStyle(bbg_gui::bbg_Dial::DialStyle::kDialDotModernStyle);
    addAndMakeVisible(amountOne);
    
    rateOneAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "freq one", rateOne);
    rateOne.setDialStyle(bbg_gui::bbg_Dial::DialStyle::kDialDotModernStyle);
    addAndMakeVisible(rateOne);
    
    amountTwoAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "depth two", amountTwo);
    amountTwo.setDialStyle(bbg_gui::bbg_Dial::DialStyle::kDialDotModernStyle);
    addAndMakeVisible(amountTwo);
    
    rateTwoAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "freq two", rateTwo);
    rateTwo.setDialStyle(bbg_gui::bbg_Dial::DialStyle::kDialDotModernStyle);
    addAndMakeVisible(rateTwo);
    
    multiplierAttachement = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "multiplier", multiplier);
    multiplier.setDialStyle(bbg_gui::bbg_Dial::DialStyle::kDialDotModernStyle);
    addAndMakeVisible(multiplier);
    
    waveformAttachement = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.treeState, "wave", waveform);
    waveform.setText("Waveform");
    waveform.addItem("Sine", 1);
    waveform.addItem("Triangle", 2);
    waveform.addItem("Sloped Square", 3);
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
    auto dialSize = getHeight() / 3.85;
    auto dialYGap = getHeight() /18.18;
    auto leftMargin = (getWidth() - (dialSize * 2)) / 2;
    
    auto multiDialSize = dialSize * 0.7;
//    auto waveformHeight =
    
    tube.setBounds(leftMargin, dialYGap, dialSize, dialSize);
    amountOne.setBounds(leftMargin, tube.getBottom() + dialYGap, dialSize, dialSize);
    amountTwo.setBounds(leftMargin, amountOne.getBottom() + dialYGap, dialSize, dialSize);
    rateOne.setBounds(amountOne.getRight(), tube.getBottom() + dialYGap, dialSize, dialSize);
    rateTwo.setBounds(amountTwo.getRight(), rateOne.getBottom() + dialYGap, dialSize, dialSize);
    multiplier.setBounds(tube.getRight() + 19, dialYGap - 8, multiDialSize, multiDialSize);
    waveform.setBounds(tube.getRight() + 19, dialYGap + 80, 90, 30);
    
}
