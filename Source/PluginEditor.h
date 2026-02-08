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
#include "VolumeFader.h"
#include "OnOffButton.h"
#include "DebugButton.h"
#include "RunTestButton.h"

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
    VolumeFader FaderLnFLeft;
    VolumeFader FaderLnFRight; 

    std::vector<std::unique_ptr<SliderStruct>> sliders;
    void addSlider(juce::String parameterID, juce::String name, ControlGroup group);
    SliderStruct* findSlider(juce::String parameterID);

    //cim
    juce::Label titleLabel;

    //szam kiiras
    juce::Label minLabel;
    juce::Label actualLabel;
    juce::Label maxLabel;

    //vizualis billenytu
    juce::MidiKeyboardComponent keyboardComponent;

    DebugButton debugButton;
    bool isDebugOn = false;
    void updateVisibility();
    void updateCabinetState();
    void updateModuleState();

    RunTestButton runTestsButton;
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
    OnOffButton cabinetOnButton;
    OnOffButton cabinetOffButton;

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

    juce::Rectangle<int> tremoloLabelBounds;
    juce::Rectangle<int> reverbLabelBounds;
    juce::Rectangle<int> linearLabelBounds;
    juce::Rectangle<int> delayLabelBounds;
    juce::Rectangle<int> numberDataBounds;

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
    //green 150, 224, 114


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModalRhodesAudioProcessorEditor)
};
