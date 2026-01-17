/*
  ==============================================================================

    CabinetSimulation.h
    Created: 16 Jan 2026 10:49:03am
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class CabinetSimulation {
public:
    CabinetSimulation() = default;
    ~CabinetSimulation() = default;

    void prepare(const juce::dsp::ProcessSpec& specs);
    void reset();
    void setParameters(float body, float brightness);

    //float processSample(float inputSample);
    void process(juce::dsp::ProcessContextReplacing<float>& context);

private:
    double sampleRate = 44100.0;

    using FilterChain = juce::dsp::ProcessorChain<
        juce::dsp::IIR::Filter<float>, //highpass, rumble
        juce::dsp::IIR::Filter<float>, //lowshelf, cabinet body
        juce::dsp::IIR::Filter<float>, //mid notch, speaker character
        juce::dsp::IIR::Filter<float>, //presence, paper cone res
        juce::dsp::IIR::Filter<float>>; //lowpass, rolloff

    FilterChain filterChainLeft;
    FilterChain filterChainRight;


    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> cabinetReflexion; //a cabinet belso reflexioi

    float reflexionMix = 0.1f;
    float currentBody = 0.5f;
    float currentBrightness = 0.5f;

    void updateFilterCoeffs();
};