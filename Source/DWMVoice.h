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

    void prepare(const juce::dsp::ProcessSpec& specs);

    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool tailOffAllowed) override;

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSample) override;

    bool canPlaySound(juce::SynthesiserSound* sound) override { return true; }
    void pitchWheelMoved(int newPitchWheelValue) override {};
    void controllerMoved(int controllerNumber, int newControllerValue) override {};

private:
    DelayLineModule dlModule;
    HammerModule hammerModule;
    bool noteCurrentlyActive = false;

    JUCE_LEAK_DETECTOR(DWMVoice)
};