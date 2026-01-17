/*
  ==============================================================================

    DelayLine.h
    Created: 19 Dec 2025 5:48:52pm
    Author:  kadar

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class DelayLineModule
{
public:
    void prepareDelayLine(const juce::dsp::ProcessSpec& specifications);

    void resetDelayLine();

    void setDelayForDelayLine(float delayInSamples, float velocity, float frequency);

    float processSample(float inputSample, float gain);

private:
    double sampleRate = 44100.0;
    float currentDelay{ 1.0f };
    float storedDelay = 100.0f;

    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> delayLine;

    std::vector<juce::dsp::FirstOrderTPTFilter<float>> allpassChain;
    juce::dsp::StateVariableTPTFilter<float> lowpassFilter;

    double getTPTPhaseAllpass(double frequency, double cutoff);
    double getTPTPhaseLow(double frequency, double cutoff, double q);
    double getTPTDelayAllpass(double frequency, double cutoff);
    double getTPTDelayLow(double frequency, double cutoff);

    juce::Random random;

    JUCE_LEAK_DETECTOR(DelayLineModule) //instance leak elkerulese vegett
};

