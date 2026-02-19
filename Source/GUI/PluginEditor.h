/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../Sound/PluginProcessor.h"
#include "../UnitTests/TestRunner.h"
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
* a teljes gui rendereleseert felelos modul
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

    //lnf classok
    CustomKnob CustomKnobLnF;
    MainKnobsLnF MainKnobsLnF;
    VolumeFader FaderLnFLeft;
    VolumeFader FaderLnFRight; 

    //fontok
    juce::Font titleFont;
    juce::Font regularFont;
    juce::Font labelFont;
    juce::Font textboxFont;

    //sliderek
    std::vector<std::unique_ptr<SliderStruct>> sliders;
    void addSlider(juce::String parameterID, juce::String name, ControlGroup group);
    SliderStruct* findSlider(juce::String parameterID);
    void placeKnob(juce::String parameterID, juce::Rectangle<int>& area, int width, bool isMain);

    //textek
    juce::Label titleLabel;
    juce::Label minLabel, actualLabel, maxLabel;
    juce::Label tremoloLabel, cabinetLabel, reverbLabel, delayLabel, inLabel, outLabel;

    //gombok, egyeb
    CustomToggleButton tremoloToggleButton, reverbToggleButton, delayToggleButton;
    OnOffButton cabinetOnButton, cabinetOffButton;
    DebugButton debugButton;
    RunTestButton runTestsButton;
    juce::TextEditor textResults;
    LevelMeter lvlMeter;
    juce::MidiKeyboardComponent keyboardComponent;

    //attachmentek
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> tremoloToggleAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> reverbToggleAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> delayToggleAttachment;


    //layout
    juce::Rectangle<int> mainSectionBounds, tremoloBounds, reverbBounds, delayBounds, linearBounds, boostBounds, cabinetBounds;
    juce::Rectangle<int> tremoloLabelBounds, reverbLabelBounds, linearLabelBounds, delayLabelBounds, numberDataBounds;

    //szinek
    juce::Colour textColour{ 252, 239, 249 };
    juce::Colour darkTextColour{ 12, 12, 12 };
    juce::Colour backgroundColour{ 3, 3, 1 };
    juce::Colour secondaryColour{ 15, 17, 26 };
    juce::Colour thirdColour{ 250, 250, 250 };
    juce::Colour accentColour{ 219, 84, 97 };


    CustomTestRunner runner;
    bool isDebugOn = false;
    
    void updateVisibility();
    void updateCabinetState();
    void updateModuleState();
    void drawSectionFrame(juce::Graphics& g, juce::Rectangle<int> bounds, bool accent); //keretek
    void drawDec(juce::Graphics& g); //vonalak

    //inicializacio segedfuggvenyek
    void initializeFonts();
    void initializeLabels();
    void initializeButtons();
    void initializeSliders();
    void initializeDebugElements();


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModalRhodesAudioProcessorEditor)
};
