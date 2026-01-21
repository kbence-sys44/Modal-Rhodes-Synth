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

    juce::dsp::StateVariableTPTFilter<float> lowpass;
    juce::dsp::StateVariableTPTFilter<float> highpass;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> gain;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> symmetryGain;

    float symmetryVariable = 1.0f;

    float envelopeFollow = 0.0f;
    float lastInputSample = 0.0f;
    float drive = 2.0f;
    float baseDelay = 14.0f;

    juce::dsp::IIR::Filter<float> bassFilter;
    juce::dsp::IIR::Filter<float> midFilter;
    juce::dsp::IIR::Filter<float> trebleFilter;
    juce::dsp::IIR::Filter<float> physicalFilter;

    juce::Random random;

    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> pickupDL{ 100 };
};