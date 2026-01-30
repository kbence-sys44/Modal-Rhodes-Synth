/*
  ==============================================================================

    CrossleyHammer.h
    Created: 20 Jan 2026 5:50:40pm
    Author:  kadar

  ==============================================================================
*/

#pragma once

#pragma once
#include <JuceHeader.h>

class CrossleyHammer
{
public:
    CrossleyHammer() = default;
    ~CrossleyHammer() = default;
	
    void prepareHammer(const juce::dsp::ProcessSpec& specs);

    void triggerHammer(float velocity, float delayInSamples, int noteNum);

    float getNextSample(float objectPos);

    bool isHammerActive() const;

    float getThump();
    void setParameters(float newStiffness, float newMass);

private:
    float sampleRate = 44100.0f;

    float hammerPos = 0.0f;
    float hammerVel = 0.0f;
    float force = 0.0f;
    bool active = false;

    int travelDelayCounter = 0;

    float mass = 0.002f; //2g
    float stiffness = 5000000.0f; //  N/m^p
    float exponent = 2.5f; // a filc linearitasa, 1 lin, 3 kemenyedo
    float damping = 0.8f; //energia elnyeles

    juce::Random random;
    juce::dsp::IIR::Filter<float> thumpFilter; //koppanas

    JUCE_LEAK_DETECTOR(CrossleyHammer)
};