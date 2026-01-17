/*
  ==============================================================================

    CabinetSimulation.cpp
    Created: 16 Jan 2026 10:49:03am
    Author:  kadar

  ==============================================================================
*/

#include "CabinetSimulation.h"

void CabinetSimulation::prepare(const juce::dsp::ProcessSpec& specs) {
    sampleRate = specs.sampleRate;
    
    juce::dsp::ProcessSpec monoSpec = specs;
    monoSpec.numChannels = 1.;

    filterChainLeft.prepare(specs);
    filterChainRight.prepare(specs);

    cabinetReflexion.prepare(specs);
    cabinetReflexion.setMaximumDelayInSamples((int)(0.02 * sampleRate)); //20ms
    cabinetReflexion.setDelay(0.0f);

    reset();
    updateFilterCoeffs();
}

void CabinetSimulation::reset() {
    filterChainLeft.reset();
    filterChainRight.reset();
    cabinetReflexion.reset();
}

void CabinetSimulation::setParameters(float body, float brightness) {
    if (currentBody != body || currentBrightness != brightness) {
        currentBody = body;
        currentBrightness = brightness;
        updateFilterCoeffs();
    }
}

void CabinetSimulation::process(juce::dsp::ProcessContextReplacing<float>& context) {
    context.getOutputBlock().multiplyBy(0.5f); //input gain

    auto& full = context.getOutputBlock();
    if (full.getNumChannels() > 0) {
        auto leftBlock = full.getSingleChannelBlock(0);
        juce::dsp::ProcessContextReplacing<float> contextL(leftBlock);
        filterChainLeft.process(contextL);
    }

    if (full.getNumChannels() > 1) {
        auto rightBlock = full.getSingleChannelBlock(1);
        juce::dsp::ProcessContextReplacing<float> contextR(rightBlock);
        filterChainRight.process(contextR);
    }

    size_t numSamples = full.getNumSamples();
    size_t numChannels = full.getNumChannels();

    for (size_t channel = 0; channel < numChannels; ++channel) {

        auto* samples = full.getChannelPointer(channel);

        for (size_t i = 0; i < numSamples; ++i) {
            float input = samples[i];

            cabinetReflexion.pushSample(channel, input);
            float reflex = cabinetReflexion.popSample(channel);

            float total = input + (reflex * 0.1f * -1.0f);

            samples[i] = std::tanh(total) * 1.5f; //sat
        }

    }
}
/*
float CabinetSimulation::processSample(float inputSample) {
    float scaledDown = inputSample * 0.5f;
    float filteredSample = filterChain.get<0>().processSample(scaledDown);
    filteredSample = filterChain.get<1>().processSample(filteredSample);
    filteredSample = filterChain.get<2>().processSample(filteredSample);
    filteredSample = filterChain.get<3>().processSample(filteredSample);
    filteredSample = filterChain.get<4>().processSample(filteredSample);

    cabinetReflexion.pushSample(0, inputSample);
    float boxSignal = cabinetReflexion.popSample(0);

    float output = inputSample + (inputSample * reflexionMix * -1.0f); //fazisforditas

    output = std::tanh(output);

    return scaledDown;
}*/

void CabinetSimulation::updateFilterCoeffs() {
    if (sampleRate <= 0.0) return;

    auto updateFilters = [&](FilterChain& chain) {
        *chain.get<0>().coefficients = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 70.0f);

        float bodyGainDB = juce::jmap(currentBody, 0.0f, 1.0f, 0.0f, 6.0f);
        float bodyGain = juce::Decibels::decibelsToGain(bodyGainDB);

        *chain.get<1>().coefficients = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 180.0f, 0.707f, bodyGain);

        *chain.get<2>().coefficients = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 600.0f, 0.6f, 1.1f);

        float presenceFr = juce::jmap(currentBrightness, 0.0f, 1.0f, 2500.0f, 4000.0f);
        *chain.get<3>().coefficients = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, presenceFr, 0.7f, 1.3f);

        *chain.get<4>().coefficients = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 7500.0f, 2.0f);
        };

    updateFilters(filterChainLeft);
    updateFilters(filterChainRight);
    

    float boxDepth = (0.0015f * sampleRate); //30-40cm melylada -> 1-2ms
    cabinetReflexion.setDelay(boxDepth);
}