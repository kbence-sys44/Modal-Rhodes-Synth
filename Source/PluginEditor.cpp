/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ModalRhodesAudioProcessorEditor::ModalRhodesAudioProcessorEditor(ModalRhodesAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    keyboardComponent(p.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    //ablak merete
    setSize (900, 500);

    //cim
    titleLabel.setText("Modal Rhodes VST", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(30.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(titleLabel);

    //keyboard
    keyboardComponent.setAvailableRange(48, 96);
    keyboardComponent.setKeyWidth(30.45);
    addAndMakeVisible(keyboardComponent);

    //segedfuggveny a sok slider miatt
    auto sliderSetup = [this](juce::Slider& slider, juce::Label& label, std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment, juce::String paramID, juce::String name) 
        {
            slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
            slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
            addAndMakeVisible(slider);

            label.setText(name, juce::dontSendNotification);
            label.setJustificationType(juce::Justification::centred);
            label.attachToComponent(&slider, false);
            addAndMakeVisible(label);

            //osszekotes az apvts-el a processzorban
            attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, paramID, slider);
        };

    sliderSetup(hardnessSlider, hardnessLabel, hardnessAttachment, "HAMMER_HARDNESS", "Hardness");
    sliderSetup(decaySlider, decayLabel, decayAttachment, "SUSTAIN_DECAY", "Decay");
    sliderSetup(releaseSlider, releaseLabel,releaseAttachment , "DAMPER_RELEASE", "Release");
    sliderSetup(symmetrySlider,symmetryLabel ,symmetryAttachment , "PICKUP_SYMMETRY", "Symmetry");

    sliderSetup(driveSlider,driveLabel,driveAttachment, "PREAMP_DRIVE", "Drive");
    sliderSetup(bassSlider,bassLabel,bassAttachment, "PREAMP_BASS", "Bass");
    sliderSetup(trebleSlider,trebleLabel,trebleAttachment, "PREAMP_TREBLE", "Treble");
    sliderSetup(tremDepthSlider,tremDepthLabel,tremDepthAttachment, "TREM_DEPTH", "Depth");
    sliderSetup(tremRateSlider,tremRateLabel,tremRateAttachment, "TREM_RATE", "Rate");
    sliderSetup(outputSlider,outputLabel,outputAttachment, "OUTPUT_GAIN", "Output");

    //teszt elemek
    addAndMakeVisible(runTestsButton);
    runTestsButton.onClick = [this] {
        runner.runAllTests();
        textResults.setText(runner.getFullResults());
    };

    addAndMakeVisible(textResults);
    textResults.setMultiLine(true);
    textResults.setReadOnly(true);
    textResults.setReturnKeyStartsNewLine(true);

    
    
}

ModalRhodesAudioProcessorEditor::~ModalRhodesAudioProcessorEditor()
{
}

//==============================================================================
void ModalRhodesAudioProcessorEditor::paint (juce::Graphics& g)
{
    //hatter
    g.fillAll(juce::Colour(0xff2b2b2b));

    //elvalaszto vonal
    g.setColour(juce::Colours::white.withAlpha(0.2f));
    g.drawLine(20, 55, getWidth() - 20, 55, 1.0f);

}

//elrendezes
void ModalRhodesAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(10);

    auto headerArea = area.removeFromTop(50);
    titleLabel.setBounds(headerArea);
    
    keyboardComponent.setBounds(area.removeFromBottom(150));
    area.removeFromBottom(20);
    area.removeFromTop(20);

    //ket sorra felosztas
    auto controlHeight = area.getHeight() / 2 - 10;

    auto topRow = area.removeFromTop(controlHeight);
    area.removeFromTop(20);
    auto bottomRow = area;

    int widthFour = topRow.getWidth() / 4;
    int widthSix = bottomRow.getWidth() / 6;

    hardnessSlider.setBounds(topRow.removeFromLeft(widthFour).reduced(10));
    decaySlider.setBounds(topRow.removeFromLeft(widthFour).reduced(10));
    releaseSlider.setBounds(topRow.removeFromLeft(widthFour).reduced(10));
    symmetrySlider.setBounds(topRow.removeFromLeft(widthFour).reduced(10));

    driveSlider.setBounds(bottomRow.removeFromLeft(widthSix).reduced(10));
    bassSlider.setBounds(bottomRow.removeFromLeft(widthSix).reduced(10));
    trebleSlider.setBounds(bottomRow.removeFromLeft(widthSix).reduced(10));
    outputSlider.setBounds(bottomRow.removeFromLeft(widthSix).reduced(10));
    tremDepthSlider.setBounds(bottomRow.removeFromLeft(widthSix).reduced(10));
    tremRateSlider.setBounds(bottomRow.removeFromLeft(widthSix).reduced(10));

    //runTestsButton.setBounds(area.removeFromTop(40));
    //textResults.setBounds(area.removeFromTop(getHeight() / 3).withTrimmedTop(10));

}
