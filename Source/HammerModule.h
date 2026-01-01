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
    void triggerHammer(float velocity, float length);

    float getNextSample();

    bool isHammerActive() const;

private:
    double sampleRate = 44100.0;
    int remainingSamples = 0;
    float currentVelocity = 0.0f;
    float lastOutput = 0.0f;
    float filterCoefficient = 0.5f;

    juce::Random random;

    JUCE_LEAK_DETECTOR(HammerModule)
};