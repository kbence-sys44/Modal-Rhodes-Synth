/*
  ==============================================================================

    ToneBarModule.h
    Created: 30 Dec 2025 3:46:42pm
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class ToneBarModule {
public:
    ToneBarModule() = default;

    void prepareToneBar(double sampleRate);
    void resetToneBar();
    void triggerToneBar(float frequency, float velocity);
    void releaseToneBar();
    float getNextSample();

    bool isToneBarActive() const;

private:

    double sampleRate = 44100.0;

    float oscState1 = 0.0f;
    float oscState2 = 0.0f;
    float oscCoeff = 0.0f;

    //envelope
    float currentAmplitude = 0.0f;
    float decayRate = 0.9999f;
    bool isReleased = false;

    double currentPhase = 0.0;
    double phaseInc = 0.0;
    float phaseFeedback = 0.0f;

    const float releaseDecay = 0.95f; //billenytu elengedesekor, gyors
    const float naturalDecay = 0.9998f; //billenytu nyomas alatt, lassu
};