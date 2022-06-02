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
    
    lfoOneLabel.setFont(juce::Font (15.0f, juce::Font::plain));
    lfoOneLabel.setJustificationType(juce::Justification::centred);
    lfoOneLabel.setColour (juce::Label::textColourId, CustomColours::creamWhite);
    addAndMakeVisible(lfoOneLabel);
    lfoTwoLabel.setFont(juce::Font (15.0f, juce::Font::plain));
    lfoTwoLabel.setJustificationType(juce::Justification::centred);
    lfoTwoLabel.setColour (juce::Label::textColourId, CustomColours::creamWhite);
    addAndMakeVisible(lfoTwoLabel);
    
    //tubeOnOff state changing tube dial and rotary fill colour
    tube.setColour(juce::Slider::thumbColourId, juce::Colours::aliceblue.darker(0.2)); // using this for when plugin is loaded
    tube.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::aliceblue.darker(0.2)); // using this for when plugin is loaded
    tubeOnOff.setButtonText("Off");
    tubeOnOff.onClick = [this]()
    {
        if(static_cast<int>(tubeOnOff.getToggleState()))
        {
            tube.setColour(juce::Slider::thumbColourId, juce::Colours::aliceblue);
            tube.setColour(juce::Slider::rotarySliderFillColourId, CustomColours::creamWhite);
            tubeOnOff.setColour(juce::TextButton::ColourIds::textColourOnId, juce::Colours::aliceblue);
            tubeOnOff.setButtonText("On");
        }
        else
        {
            tube.setColour(juce::Slider::thumbColourId, juce::Colours::aliceblue.darker(0.2));
            tube.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::aliceblue.darker(0.2));
            tubeOnOff.setButtonText("Off");
        }
    };
    
    //Resizing
    setResizable(true, true);
    setResizeLimits(270, 495, 324, 594); // max limits are min * 1.2
    getConstrainer()->setFixedAspectRatio(0.55);
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (270, 495);
}

MyTremoloAudioProcessorEditor::~MyTremoloAudioProcessorEditor()
{
}

//==============================================================================
void MyTremoloAudioProcessorEditor::paint (juce::Graphics& g)
{
    juce::Rectangle<int> background = getLocalBounds();
    g.setGradientFill(juce::ColourGradient::vertical(juce::Colours::gold, getHeight() - getHeight(), juce::Colours::orange.brighter(), getHeight()));
    g.fillRect(background);
    
    //Line
    auto lineX = (getWidth() / 2) + ((getHeight() / 4) / 2);
    auto lineYone = getHeight() * 0.6465;
    auto lineYtwo = getHeight() * 0.7112;
    g.setColour (CustomColours::blackGrey.brighter(0.8));
    
    juce::Line<float> line (juce::Point<float> (lineX, lineYone),
                            juce::Point<float> (lineX, lineYtwo));

     g.drawLine (line, 1.0f);
}

void MyTremoloAudioProcessorEditor::resized()
{
    auto topMargain = getHeight() / 8;
    auto dialSize = getHeight() / 4.0;
    auto dialYGap = getHeight() / 18.18;
    auto leftMargin = (getWidth() - (dialSize * 2)) / 2;
    auto extraYDialGap = getHeight() / 50;
    
    auto multiDialSize = dialSize * 0.8;
    auto multiXGap = (dialSize - multiDialSize) / 2;
    auto mutliYGap = getHeight() * 0.1255;
    
    auto waveformWidth = dialSize * 0.6;
    auto waveformHeight = dialSize * 0.19;
    auto waveformXGap = (dialSize - waveformWidth) / 2;
    auto waveformYGap = getHeight() * 0.0146;
    
    auto tubeOnWidth = dialSize * 0.25;
    auto tubeOnHeight = dialSize * 0.15;
    auto tubeOnXGap = (dialSize - tubeOnWidth) / 2;
    auto tubeOnYGap = getHeight() * 0.05;
    
    auto lfoLabelWidth = getWidth() * 0.3334;
    auto lfoLabelHeight = getHeight() * 0.05;
    auto lfoLabelXPos = (getWidth() / 2) - (lfoLabelWidth / 2);
    auto lfoOneYPos = topMargain + (dialSize * 2);
    auto lfoTwoYPos = topMargain + (dialSize * 3) + dialYGap;
    
    tube.setBounds(leftMargin, topMargain, dialSize, dialSize);
    tubeOnOff.setBounds(leftMargin + tubeOnXGap, tube.getBottom() - tubeOnYGap, tubeOnWidth, tubeOnHeight);
    multiplier.setBounds(tube.getRight() + multiXGap, mutliYGap, multiDialSize, multiDialSize);
    waveform.setBounds(tube.getRight() + waveformXGap, multiplier.getBottom() - waveformYGap, waveformWidth, waveformHeight);
    amountOne.setBounds(leftMargin, tube.getBottom() + dialYGap + extraYDialGap, dialSize, dialSize);
    amountTwo.setBounds(leftMargin, amountOne.getBottom() + dialYGap, dialSize, dialSize);
    rateOne.setBounds(amountOne.getRight(), tube.getBottom() + dialYGap + extraYDialGap, dialSize, dialSize);
    rateTwo.setBounds(amountTwo.getRight(), rateOne.getBottom() + dialYGap, dialSize, dialSize);
    
    lfoOneLabel.setBounds(lfoLabelXPos, lfoOneYPos, lfoLabelWidth, lfoLabelHeight);
    lfoTwoLabel.setBounds(lfoLabelXPos, lfoTwoYPos, lfoLabelWidth, lfoLabelHeight);
    
}
