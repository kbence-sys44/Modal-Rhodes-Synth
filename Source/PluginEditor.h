/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "TestRunner.h"

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

    //cim
    juce::Label titleLabel;

    //vizualis billenytu
    juce::MidiKeyboardComponent keyboardComponent;

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModalRhodesAudioProcessorEditor)
};
