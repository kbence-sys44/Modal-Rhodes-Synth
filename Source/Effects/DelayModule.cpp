/*
  ==============================================================================

    DelayModule.cpp
    Created: 2 Feb 2026 4:55:39pm
    Author:  kadar

  ==============================================================================
*/

#include "DelayModule.h"


void DelayModule::prepare(const juce::dsp::ProcessSpec& specs) {
    sampleRate = specs.sampleRate;

    delayLine.prepare(specs);

    for (auto& filter : filters) {
        filter.prepare(specs);
        filter.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
    }

    delayTimeSmooth.reset(sampleRate, 0.05);
    feedbackSmooth.reset(sampleRate, 0.05);
    mixSmooth.reset(sampleRate, 0.05);

    reset();
}

void DelayModule::reset() {

    delayLine.reset();
    for (auto& filter : filters) filter.reset();

}

void DelayModule::setParameters(float timeInMs, float feedback, float mixRatio, float toneHz) {

    float delayInSamples = (timeInMs / 1000.0f) * sampleRate;

    if (delayInSamples < 1.0f) delayInSamples = 1.0f;
    if (delayInSamples > sampleRate * maxDelayInSeconds) delayInSamples = sampleRate * maxDelayInSeconds;

    delayTimeSmooth.setTargetValue(delayInSamples);
    feedbackSmooth.setTargetValue(juce::jlimit(0.0f, 0.95f, feedback));
    mixSmooth.setTargetValue(juce::jlimit(0.0f, 1.0f, mixRatio));

    for (auto& filter : filters) {
        filter.setCutoffFrequency(toneHz);
    }

}
void DelayModule::process(juce::dsp::ProcessContextReplacing<float>& context) {

    auto& processBlock = context.getOutputBlock();
    size_t numChannels = processBlock.getNumChannels();
    size_t numSamples = processBlock.getNumSamples();

    for (size_t i = 0; i < numSamples; ++i) {

        float currentDelay = delayTimeSmooth.getNextValue();
        float currentFeedback = feedbackSmooth.getNextValue();
        float currentMix = mixSmooth.getNextValue();

        for (size_t ch = 0; ch < numChannels; ++ch) {

            float* channelData = processBlock.getChannelPointer(ch);
            float inputSample = channelData[i];

            float delayedSample = delayLine.popSample((int)ch, currentDelay, true);
            delayedSample = filters[ch].processSample(0, delayedSample);

            float feedbackSample = inputSample + (delayedSample * currentFeedback);

            if (feedbackSample > 2.0f) feedbackSample = 2.0f;
            if (feedbackSample < -2.0f) feedbackSample = -2.0f;

            delayLine.pushSample((int)ch, feedbackSample);

            channelData[i] = (inputSample * (1.0f - currentMix)) + (delayedSample * currentMix);

        }

    }

}