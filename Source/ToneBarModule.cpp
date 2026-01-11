/*
  ==============================================================================

    ToneBarModule.cpp
    Created: 30 Dec 2025 3:46:42pm
    Author:  kadar

  ==============================================================================
*/

#include "ToneBarModule.h"

void ToneBarModule::prepareToneBar(double sampleRate) {
    this->sampleRate = sampleRate;
    resetToneBar();
}

void ToneBarModule::resetToneBar() {
    currentAmplitude = 0.0f;
    oscState1 = 0.0f;
    oscState2 = 0.0f;
    oscCoeff = 0.0f;
    isReleased = false;

}

void ToneBarModule::triggerToneBar(float frequency, float velocity) {
    //coeff kiszamitasa : 2*cos(2*PI*freq/sr)
    double omega = juce::MathConstants<double>::twoPi * frequency / sampleRate;
    oscCoeff = (float)(2.0 * std::cos(omega));

    oscState1 = 0.0f;
    oscState2 = std::sin(omega)*velocity;

    currentAmplitude = velocity;
    decayRate = naturalDecay;
    isReleased = false;
}

void ToneBarModule::releaseToneBar() {
    isReleased = true;
    decayRate = releaseDecay;
}

float ToneBarModule::getNextSample() {

    if (currentAmplitude < 0.0001f) return 0.0f;

    float nextVal = (oscCoeff * oscState2) - oscState1;

    oscState1 = oscState2;
    oscState2 = nextVal;

    oscState1 *= decayRate;
    oscState2 *= decayRate;

    currentAmplitude *= decayRate;

    return nextVal;

    //regi verzio
   /* double bend = 0.0;
    if (sampleCountSinceTrigger < 1000) {
        bend = (1.0 - (sampleCountSinceTrigger / 1000.0)) * 0.05; // 5%
    }
    sampleCountSinceTrigger++;

    float tonebarSample = std::sin(currentPhase * (1.0 + bend)) * currentAmplitude;

    currentPhase += phaseIncrement;
    if (currentPhase >= juce::MathConstants<double>::twoPi) {
        currentPhase -= juce::MathConstants<double>::twoPi;
    }

    currentAmplitude *= decayRate;

    return tonebarSample;*/
}

bool ToneBarModule::isToneBarActive() const
{
    return currentAmplitude > 0.0001f;
}