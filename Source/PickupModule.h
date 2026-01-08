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

    void preparePickup(double sampleRate);
    void reset();

    void setDrive(float newDrive);

    float processSignal(float inputSample);

private:
    double sampleRate = 44100.0;

    float lastInputSample = 0.0f;
    float drive = 2.5f;

    juce::dsp::IIR::Filter<float> toneFilter;
    juce::dsp::DelayLine<float> pickupDL{ 100 };
};