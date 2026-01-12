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

    void preparePickup(const juce::dsp::ProcessSpec& specs);
    void reset();

    void setDrive(float newDrive);
    void setBassGain(float newGain);
    void setBaseDelay(float newDelay);

    float processSignal(float inputSample);

private:
    double sampleRate = 44100.0;

    float envelopeFollow = 0.0f;
    float lastInputSample = 0.0f;
    float drive = 2.5f;
    float baseDelay = 14.0f;

    juce::dsp::IIR::Filter<float> bassFilter;
    juce::dsp::IIR::Filter<float> midFilter;
    juce::dsp::IIR::Filter<float> trebleFilter;
    juce::dsp::IIR::Filter<float> physicalFilter;

    juce::Random random;

    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> pickupDL{ 100 };
};