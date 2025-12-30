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

    void setDelayForDelayLine(float delayInSamples);

    float processSample(float inputSample, float gain);

private:
    double sampleRate{ 0.0 };
    float currentDelay{ 1.0f };

    juce::dsp::DelayLine<float> delayLine;

    juce::dsp::IIR::Filter<float> IIRfilter; //csillapításért felelõs szûrõ

    juce::dsp::IIR::Filter<float> allpassFilter;

    JUCE_LEAK_DETECTOR(DelayLineModule) //instance leak elkerülése végett
};

