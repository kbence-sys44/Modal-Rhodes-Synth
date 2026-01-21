/*
  ==============================================================================

    ToneBarModule.h
    Created: 30 Dec 2025 3:46:42pm
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <array>

class ToneBarModule {
public:
    ToneBarModule() = default;
    ~ToneBarModule() = default;

    void prepare(const juce::dsp::ProcessSpec& specs);

    void reset();

    float processSample(float inputSample);

private:

    float sampleRate = 44100.0f;

    std::array<juce::dsp::StateVariableTPTFilter<float>, 3> filters;

    float gainSub = 1.0f;
    float gainLow = 1.0f;
    float gainHigh = 1.0f;

   /* float oscState1 = 0.0f;
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

    */
};