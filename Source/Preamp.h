/*
  ==============================================================================

    Preamp.h
    Created: 11 Jan 2026 8:21:45pm
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Preamp {
public:
    Preamp() = default;
    
    void prepare(const juce::dsp::ProcessSpec& specs);
    void reset();

    void setDrive(float newDrive);
    void setBassGain(float newBassGain);
    void setTrebleGain(float newTrebleGain);
    void setVolume(float newVolume);

    float processSample(float inputSample);

private:
    void updateFilters();

    float sampleRate = 44100.0f;

    float drive = 2.0f;
    float outputVolume = 2.0f;
    float bassGain = 1.3f;
    float trebleGain = 0.7f;

    juce::dsp::IIR::Filter<float> bassFilter;
    juce::dsp::IIR::Filter<float> trebleFilter;

    juce::dsp::StateVariableTPTFilter<float> dcBlocker;

    //float prevInput = 0.0f;
    //float prevOutput = 0.0f;
};