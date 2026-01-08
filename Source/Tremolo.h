/*
  ==============================================================================

    Tremolo.h
    Created: 8 Jan 2026 6:38:11pm
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct Stereo {
    float left;
    float right;
};

class Tremolo {
public:
    Tremolo() = default;

    void prepare(double sampleRate);
    
    void setTremRate(float rateInHz);

    void setDepth(float depth);

    Stereo process(float inputSample);

private:
    double sampleRate = 44100.0;

    double currentPhase = 0.0;
    double phaseInc = 0.0;

    float currentDepth = 0.0f;
    float currentRate = 0.0f;

};