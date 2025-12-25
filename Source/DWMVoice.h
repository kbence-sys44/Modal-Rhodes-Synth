/*
  ==============================================================================

    DWMVoice.h
    Created: 24 Dec 2025 3:43:06pm
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "HammerModule.h"
#include "DelayLineModule.h"

class DWMVoice : public juce::SynthesiserVoice
{
public:
    DWMVoice() {}

    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool tailOffAllowed) override;

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSample) override;

private:
    DelayLineModule dlModule;
    HammerModule hammerModule;
    bool noteCurrentlyActive = false;

    JUCE_LEAK_DETECTOR(DWMVoice)
};