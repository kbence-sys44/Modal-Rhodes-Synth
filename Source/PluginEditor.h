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
#include "CustomKnob.h"

//==============================================================================
/**
*/
class ModalRhodesAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ModalRhodesAudioProcessorEditor(ModalRhodesAudioProcessor&);
    ~ModalRhodesAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:

    ModalRhodesAudioProcessor& audioProcessor;

    CustomKnob CustomKnobLnF;

    //cim
    juce::Label titleLabel;

    //vizualis billenytu
    juce::MidiKeyboardComponent keyboardComponent;

    juce::TextButton debugButton{ "DEBUG" };
    bool isDebugOn = false;
    void updateVisibility();

    juce::TextButton runTestsButton{ "Run all tests" };
    juce::TextEditor textResults;
    CustomTestRunner runner;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    //fizikai parameterek
    juce::Slider hardnessSlider;
    juce::Label hardnessLabel;
    std::unique_ptr<SliderAttachment> hardnessAttachment;

    juce::Slider decaySlider;
    juce::Label decayLabel;
    std::unique_ptr<SliderAttachment> decayAttachment;

    juce::Slider releaseSlider;
    juce::Label releaseLabel;
    std::unique_ptr<SliderAttachment> releaseAttachment;

    juce::Slider symmetrySlider;
    juce::Label symmetryLabel;
    std::unique_ptr<SliderAttachment> symmetryAttachment;

    //elektronika/ effektek
    juce::Slider driveSlider;
    juce::Label driveLabel;
    std::unique_ptr<SliderAttachment> driveAttachment;

    juce::Slider bassSlider;
    juce::Label bassLabel;
    std::unique_ptr<SliderAttachment> bassAttachment;

    juce::Slider trebleSlider;
    juce::Label trebleLabel;
    std::unique_ptr<SliderAttachment> trebleAttachment;

    juce::Slider outputSlider;
    juce::Label outputLabel;
    std::unique_ptr<SliderAttachment> outputAttachment;

    juce::Slider tremDepthSlider;
    juce::Label tremDepthLabel;
    std::unique_ptr<SliderAttachment> tremDepthAttachment;

    juce::Slider tremRateSlider;
    juce::Label tremRateLabel;
    std::unique_ptr<SliderAttachment> tremRateAttachment;

    juce::Slider reverbWetSlider;
    juce::Label reverbWetLabel;
    std::unique_ptr<SliderAttachment> reverbWetAttachment;

    juce::Slider reverbDrySlider;
    juce::Label reverbDryLabel;
    std::unique_ptr<SliderAttachment> reverbDryAttachment;

    //gombok
    juce::Label tremoloLabel;
    CustomToggleButton tremoloToggleButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> tremoloToggleAttachment;

    juce::Label reverbLabel;
    CustomToggleButton reverbToggleButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> reverbToggleAttachment;

    juce::Label cabinetLabel;
    CustomToggleButton cabinetToggleButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> cabinetToggleAttachment;

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


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModalRhodesAudioProcessorEditor)
};
