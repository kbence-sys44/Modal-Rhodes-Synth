/*
  ==============================================================================

    ModalTine.cpp
    Created: 18 Jan 2026 3:14:54pm
    Author:  kadar

  ==============================================================================
*/

#include "ModalTine.h"

void ModalTine::prepare(const juce::dsp::ProcessSpec& specs) {
    sampleRate = specs.sampleRate;

    modes[0].frequencyRatio = 1.0f;
    modes[1].frequencyRatio = 6.27f;
    modes[2].frequencyRatio = 17.55f;
    modes[3].frequencyRatio = 34.39f;

    modes[4].filter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    modes[4].frequencyRatio = 0.0f;

    for (auto& mode : modes) {
        mode.prepare(specs);
    }
}
void ModalTine::reset() {
    for (auto& mode : modes) {
        mode.reset();
    }
}
void ModalTine::triggerTine(float frequency, float velocity) {

    for (auto& mode : modes) {
        mode.filter.reset();
    }

    //alaphang
    float f0 = frequency * modes[0].frequencyRatio;
    float decay0 = juce::jmap(frequency, 40.0f, 2000.0f, 15.0f, 3.0f);

    modes[0].filter.setCutoffFrequency(f0);
    modes[0].filter.setResonance(calculateQ(f0, decay0) * 0.4f);
    modes[0].gain = 1.0f;

    //bell
    float f1 = frequency * modes[1].frequencyRatio;
    float decay1 = juce::jmap(frequency, 40.0f, 3000.0f, 5.0f, 0.5f);

    if (f1 > sampleRate * 0.45f) modes[1].gain = 0.0f; //nyquist limit
    else {
        modes[1].filter.setCutoffFrequency(f1);
        modes[1].filter.setResonance(calculateQ(f1, decay1));
        modes[1].gain = 0.5f;
    }

    //mode 2 - metallic click
    float f2 = frequency * modes[2].frequencyRatio;
    float decay2 = 0.15f;


    if (f2 > sampleRate * 0.45f) modes[2].gain = 0.0f;
    else {
        modes[2].filter.setCutoffFrequency(f2);
        modes[2].filter.setResonance(calculateQ(f2, decay2));
        modes[2].gain = 0.2f;
    }

    //air
    float f3 = frequency * modes[3].frequencyRatio;
    float decay3 = 0.08f;

    if (f3 > sampleRate * 0.45f) modes[3].gain = 0.0f;
    else {
        modes[3].filter.setCutoffFrequency(f3);
        modes[3].filter.setResonance(calculateQ(f3, decay3));
        modes[3].gain = 0.1f;
    }

    //body
    float bodyFreq = 200.0f;
    float bodyQ = 0.1f;

    
    modes[4].filter.setCutoffFrequency(bodyFreq);
    modes[4].filter.setResonance(bodyQ);
    modes[4].gain = 0.1f;

    //reset();
}
float ModalTine::processSample(float inputSample) {
    float output = 0.0f;

    for (auto& mode : modes) {
        if (mode.gain > 0.0001f) {
            float filtered = mode.filter.processSample(0, inputSample);
            output += filtered * mode.gain;
        }
    }

    return output * 1.0f;
}

float ModalTine::calculateQ(float frequency, float decayTime) {
    if (decayTime < 0.001f) decayTime = 0.001f;
    if (frequency < 20.0f) frequency = 20.0f;

    float rad = 2.0f * juce::MathConstants<float>::pi * frequency;
    float q = decayTime * rad / 6.91f;

    return juce::jlimit(0.5f, 800.0f, q);
}

void ModalTine::damp() {
    for (auto& mode : modes) {
        float dampedQ = calculateQ(mode.filter.getCutoffFrequency(), 0.08f);
        mode.filter.setResonance(dampedQ);
    }
}