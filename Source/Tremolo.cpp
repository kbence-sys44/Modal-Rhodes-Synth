/*
  ==============================================================================

    Tremolo.cpp
    Created: 8 Jan 2026 6:38:11pm
    Author:  kadar

  ==============================================================================
*/

#include "Tremolo.h"

void Tremolo::prepare(double sr) {
    sampleRate = sr;
    currentPhase = 0.0;
    

    //alap ertekek
    setTremRate(1.0f);
    setDepth(0.8f);
}

void Tremolo::setTremRate(float rateInHz) {
    currentRate = rateInHz;

    phaseInc = (currentRate / sampleRate) * juce::MathConstants<double>::twoPi;

}

void Tremolo::setDepth(float depth) {
    currentDepth = juce::jlimit(0.0f, 1.0f, depth);
}

//mintankent egy lfo alapjan kiszamolja hogy mikor melyik oldalt legyen hangosabb a hang
Stereo Tremolo::process(float inputSample) {

    float lfo = (float)std::sin(currentPhase);

    currentPhase += phaseInc;

    if (currentPhase >= juce::MathConstants<double>::twoPi) currentPhase -= juce::MathConstants<double>::twoPi;

    float modLeft = (lfo + 1.0f) * 0.5f;
    float modRight = 1.0f - modLeft;

    float gainLeft = 1.0f - (currentDepth * modRight);
    float gainRight = 1.0f - (currentDepth * modLeft);

    return { inputSample * gainLeft, inputSample * gainRight };
}
