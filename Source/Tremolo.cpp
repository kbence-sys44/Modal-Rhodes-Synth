/*
  ==============================================================================

    Tremolo.cpp
    Created: 8 Jan 2026 6:38:11pm
    Author:  kadar

  ==============================================================================
*/

//sztereo pan effekt, a bal es jobb csatorna gaint modulalja oda vissza
//a rhodes zongorak jellegzetessege

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

    phaseInc = (currentRate / (float)sampleRate) * juce::MathConstants<double>::twoPi;
}

void Tremolo::setDepth(float depth) {
    currentDepth = juce::jlimit(0.0f, 1.0f, depth);
}

void Tremolo::setShape(float newShape) {
    shape = juce::jlimit(1.0f, 10.0f, newShape);
}

void Tremolo::setTremoloState(bool newState) {
    tremoloEnabled = newState;
}

//mintankent egy lfo alapjan kiszamolja hogy mikor melyik oldalt legyen hangosabb a hang
Stereo Tremolo::process(float inputSample) {

    float lfo = (float)std::sin(currentPhase);
    float shapedLfo = std::tanh(lfo * shape);
    float normalizer = std::tanh(shape);
    shapedLfo /= normalizer;

    currentPhase += phaseInc;

    if (currentPhase >= juce::MathConstants<double>::twoPi) currentPhase -= juce::MathConstants<double>::twoPi;

    float modLeft = (shapedLfo + 1.0f) * 0.5f;
    float modRight = 1.0f - modLeft;

    float gainLeft = 1.0f - (currentDepth * modRight);
    float gainRight = 1.0f - (currentDepth * modLeft);

    return { inputSample * gainLeft, inputSample * gainRight };
}
