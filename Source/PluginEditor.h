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
class RhodesDWMAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    RhodesDWMAudioProcessorEditor (RhodesDWMAudioProcessor&);
    ~RhodesDWMAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:

    RhodesDWMAudioProcessor& audioProcessor;

    //vizualis billenytu
    juce::MidiKeyboardComponent keyboardComponent;

    juce::TextButton runTestsButton{ "Run all tests" };
    juce::TextEditor textResults;
    CustomTestRunner runner;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RhodesDWMAudioProcessorEditor)
};
