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

    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModalRhodesAudioProcessorEditor)
};
