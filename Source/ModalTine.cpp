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

    //harmonikusok
    modes[0].frequencyRatio = 0.5f; 
    modes[1].frequencyRatio = 1.0f;//alaphang
    modes[2].frequencyRatio = 2.0f;
    modes[3].frequencyRatio = 3.0f;
    modes[4].frequencyRatio = 4.0f;
    modes[5].frequencyRatio = 5.0f;

    //inharmonikusok
    modes[6].frequencyRatio = 6.27f; //todo - ez frekvencia alapjan mas 
    modes[7].frequencyRatio = 17.55f;
    modes[8].frequencyRatio = 34.39f;

    //karakter, csillogas
    modes[9].frequencyRatio = 7.0f;
    modes[10].frequencyRatio = 8.0f;
    modes[11].frequencyRatio = 9.0f;
    modes[12].frequencyRatio = 10.0f;
    modes[13].frequencyRatio = 12.0f;
    modes[14].frequencyRatio = 14.0f;
    modes[15].frequencyRatio = 16.0f;

    //fix frekvencia
    modes[16].frequencyRatio = 1.0f;

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
    float baseDecay = juce::jmap(frequency, 40.0f, 2000.0f, 8.0f, 1.5f);

    float velMultiplier = 0.5f + (velocity * 1.5f);

    configSpikes(0, frequency, 1.0f, 0.8f, 1.0f); //c2
    configSpikes(1, frequency, baseDecay, 2.3f, 1.0f); //c3
    configSpikes(2, frequency, baseDecay, 1.3f * velMultiplier, 0.8f); //c4
    configSpikes(3, frequency, baseDecay, 0.1f * velMultiplier, 0.6f); //g4
    configSpikes(4, frequency, baseDecay, 0.1f * velMultiplier, 0.5f); //c5
    configSpikes(5, frequency, baseDecay, 0.2f * velMultiplier, 0.4f); //e5

    configSpikes(6, frequency, baseDecay, 0.10f, 0.35f); // 627
    configSpikes(7, frequency, 0.15f, 0.05f, 1.0f); // 1755
    configSpikes(8, frequency, 0.08f, 0.03f, 1.0f); // 3439

    configSpikes(9, frequency, baseDecay, 0.10f, 0.3f); //a#5
    configSpikes(10, frequency, 0.25f, 0.05f, 1.0f); //c6
    configSpikes(11, frequency, 0.20f, 0.05f, 1.0f); //d6
    configSpikes(12, frequency, 0.18f, 0.04f, 1.0f); //e6
    configSpikes(13, frequency, 0.15f, 0.03f, 1.0f); //g6
    configSpikes(14, frequency, 0.12f, 0.015f, 1.0f); //a#6
    configSpikes(15, frequency, 0.10f, 0.005f, 1.0f); //c7

    configSpikes(16, 80.0f, baseDecay, 1.3f, 1.0f);




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

void ModalTine::configSpikes(int index, float frequency, float decay, float gain, float QMulti) {

    float f = frequency * modes[index].frequencyRatio;

    if (f > sampleRate * 0.48f) {
        modes[index].gain = 0.0f;
        return;
    }

    modes[index].filter.setCutoffFrequency(f);
    modes[index].filter.setResonance(calculateQ(f,decay) * QMulti);

    float lowEndBoost = 1.0f;
    if (f < 300.0f) {
        lowEndBoost = 300.0f / f;
    }
    lowEndBoost = juce::jmin(lowEndBoost, 12.0f);

    modes[index].gain = gain * lowEndBoost;

}

float ModalTine::calculateQ(float frequency, float decayTime) {
    if (decayTime < 0.001f) decayTime = 0.001f;
    if (frequency < 20.0f) frequency = 20.0f;

    float rad = 2.0f * juce::MathConstants<float>::pi * frequency;

    float qMultiplier = juce::jmap(frequency, 40.0f, 1500.0f, 0.1f, 1.0f);
    float q = decayTime * rad / 6.91f * qMultiplier;

    return juce::jlimit(0.5f, 800.0f, q);
}

void ModalTine::damp() {
    for (auto& mode : modes) {
        float dampedQ = calculateQ(mode.filter.getCutoffFrequency(), 0.08f);
        mode.filter.setResonance(dampedQ);
    }
}