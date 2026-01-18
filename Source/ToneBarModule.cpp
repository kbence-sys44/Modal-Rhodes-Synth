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

    phaseInc = (frequency / sampleRate) * juce::MathConstants<double>::twoPi;
    currentPhase = 0.0;
    currentAmplitude = velocity;

    phaseFeedback = velocity * 1.5f;

    float decaySecs = 0.0f;
    if (frequency < 100.0f) {
        decaySecs = 6.0f + (velocity * 2.0f);
    }
    else if (frequency < 400.0f) {
        decaySecs = 3.0f + (velocity * 1.0f);
    }
    else {
        decaySecs = 0.8f + (velocity * 0.5f);
    }
    decayRate = std::pow(0.001f, 1.0f / (sampleRate * decaySecs));
    isReleased = false;
    currentAmplitude *= 2.0f;
}

void ToneBarModule::releaseToneBar() {
    isReleased = true;
    decayRate = releaseDecay;
}

float ToneBarModule::getNextSample() {

    if (currentAmplitude < 0.0001f) return 0.0f;

    float rawSine = std::sin(currentPhase);
    float output = std::sin(currentPhase);

    currentPhase += phaseInc;
    if (currentPhase >= juce::MathConstants<double>::twoPi) {
        currentPhase -= juce::MathConstants<double>::twoPi;
    }

    output *= currentAmplitude;
    currentAmplitude *= decayRate;
    phaseFeedback *= 0.9995f;

    return output;
}

bool ToneBarModule::isToneBarActive() const
{
    return currentAmplitude > 0.0001f;
}