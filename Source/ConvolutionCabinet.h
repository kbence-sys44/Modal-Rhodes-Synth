/*
  ==============================================================================

    ConvolutionCabinet.h
    Created: 17 Jan 2026 8:32:00pm
    Author:  kadar

  ==============================================================================
*/

#pragma once
#pragma once
#include <JuceHeader.h>

class ConvolutionCabinet {
public:
    ConvolutionCabinet() = default;
    ~ConvolutionCabinet() = default;

    void prepare(const juce::dsp::ProcessSpec& specs);
    void reset();
    void setParameters(float body, float brightness);
    void process(juce::dsp::ProcessContextReplacing<float>& context);

private:
    double sampleRate = 44100.0;
    float makeupGain = 5.5f;

    juce::dsp::Convolution cabinetConv;
};