/*
  ==============================================================================

    PickupModule.h
    Created: 2 Jan 2026 7:19:57pm
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class PickupModule {
public:
    PickupModule() = default;
    ~PickupModule() = default;

    void prepare(const juce::dsp::ProcessSpec& specs);
    void reset();

    float processSample(float inputSample);
    
    void setParameters(float gainDB, float symmetryDB, float lowPassCutoff);
    
    void setFrequency(float frequency);
    

private:
    double sampleRate = 44100.0;
    float symmetryVariable = 1.0f;

    juce::dsp::StateVariableTPTFilter<float> lowpass;
    juce::dsp::StateVariableTPTFilter<float> highpass;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> gain;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> symmetryGain;

};