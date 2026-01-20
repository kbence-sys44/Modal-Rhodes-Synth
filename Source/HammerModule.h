/*
  ==============================================================================

    HammerModule.h
    Created: 23 Dec 2025 4:16:00pm
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class HammerModule
{
public:
	HammerModule() = default;
	
    void prepareHammer(double sRate);
    void triggerHammer(float velocity, float length, int noteNum);

    float getNextSample();

    bool isHammerActive() const;
    float getVelocity() const;
    float getThump();

private:
    double sampleRate = 44100.0;
    int remainingSamples = 0;
    float currentVelocity = 0.0f;
    float lastOutput = 0.0f;
    float filterCoefficient = 0.5f;
    float totalDur = 0.0f;

    juce::dsp::StateVariableTPTFilter<float> hammerFilter;

    juce::dsp::StateVariableTPTFilter<float> thumpFilter;//puffanas filter
    float thumpEnv = 0.0f;
    float thumpDecay = 0.0f;

    juce::Random random;

    JUCE_LEAK_DETECTOR(HammerModule)
};