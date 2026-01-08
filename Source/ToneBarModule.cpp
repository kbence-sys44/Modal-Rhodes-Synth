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
    currentPhase = 0.0;
    phaseIncrement = 0.0;
    isReleased = false;

}

void ToneBarModule::triggerToneBar(float frequency, float velocity) {
    //lepesek kiszamitasa : 2*PI*freq/sr

    phaseIncrement = (frequency / sampleRate) * juce::MathConstants<double>::twoPi;
    sampleCountSinceTrigger = 0;

    currentPhase = 0.0;
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


    //hajlitas a hang elején
    double bend = 0.0;
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

    return tonebarSample;
}

bool ToneBarModule::isToneBarActive() const
{
    return currentAmplitude > 0.0001f;
}