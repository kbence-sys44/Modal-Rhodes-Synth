/*
  ==============================================================================

    SliderBundle.h
    Created: 5 Feb 2026 7:08:18pm
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

enum class ControlGroup {
    Main,
    Tremolo,
    Reverb,
    Delay,
    Preamp,
    Output,
    Cabinet
};

struct SliderStruct {
    std::unique_ptr<juce::Slider> slider;
    std::unique_ptr<juce::Label> label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    juce::String parameterID;
    ControlGroup group;

    void setVisible(bool shouldBeVis) {
        if (slider) slider->setVisible(shouldBeVis);
        if (label) label->setVisible(shouldBeVis);
    }

    void setLnF(juce::LookAndFeel* lnf) {
        if (slider) slider->setLookAndFeel(lnf);
    }

};