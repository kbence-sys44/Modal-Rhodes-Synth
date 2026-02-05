/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "TestRunner.h"
#include "CustomToggleButton.h"
#include "CustomKnobLnF.h"
#include "MainKnobsLnF.h"
#include "LevelMeter.h"
#include "SliderBundle.h"

//==============================================================================
/**
*/


class ModalRhodesAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    ModalRhodesAudioProcessorEditor(ModalRhodesAudioProcessor&);
    ~ModalRhodesAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void paintOverChildren(juce::Graphics& g) override;
    void timerCallback() override;

private:

    ModalRhodesAudioProcessor& audioProcessor;

    CustomKnob CustomKnobLnF;
    MainKnobsLnF MainKnobsLnF;
    LevelMeter lvlMeter;

    std::vector<std::unique_ptr<SliderStruct>> sliders;
    void addSlider(juce::String parameterID, juce::String name, ControlGroup group);
    SliderStruct* findSlider(juce::String parameterID);

    //cim
    juce::Label titleLabel;

    //vizualis billenytu
    juce::MidiKeyboardComponent keyboardComponent;

    juce::TextButton debugButton{ "DEBUG" };
    bool isDebugOn = false;
    void updateVisibility();
    void updateCabinetState();

    juce::TextButton runTestsButton{ "Run all tests" };
    juce::TextEditor textResults;
    CustomTestRunner runner;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    //gombok
    juce::Label tremoloLabel;
    CustomToggleButton tremoloToggleButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> tremoloToggleAttachment;

    juce::Label reverbLabel;
    CustomToggleButton reverbToggleButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> reverbToggleAttachment;

    juce::Label cabinetLabel;
    juce::TextButton cabinetOnButton;
    juce::TextButton cabinetOffButton;

    juce::Label delayLabel;
    CustomToggleButton delayToggleButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> delayToggleAttachment;

    juce::Label inLabel;
    juce::Label outLabel;


    //keretekhez

    juce::Rectangle<int> mainSectionBounds;
    juce::Rectangle<int> tremoloBounds;
    juce::Rectangle<int> reverbBounds;
    juce::Rectangle<int> delayBounds;
    juce::Rectangle<int> linearBounds;
    juce::Rectangle<int> boostBounds;
    juce::Rectangle<int> cabinetBounds;

    juce::Colour textColour { 252, 239, 249 };
    juce::Colour darkTextColour { 12, 12, 12 };

    juce::Colour backgroundColour{ 3, 3, 1 };
    juce::Colour secondaryColour{ 15, 17, 26 };
    juce::Colour thirdColour{ 250, 250, 250 };
    juce::Colour accentColour{ 219, 84, 97 };

    //brgiht snow 250, 250, 250
    //alice blue 227, 242, 253
    //lobster pink 219, 84, 97
    //ink black 15, 17, 26
    //black 3, 3, 1


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModalRhodesAudioProcessorEditor)
};
