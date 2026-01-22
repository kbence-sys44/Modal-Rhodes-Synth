/*
  ==============================================================================

    Preamp.h
    Created: 11 Jan 2026 8:21:45pm
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Preamp {
public:
    Preamp() = default;
    ~Preamp() = default;
    
    void prepare(const juce::dsp::ProcessSpec& specs);
    void reset();

    void process(juce::dsp::ProcessContextReplacing<float>& context);

    void setDrive(float newDrive);
    void setBassGain(float newBassGain);
    void setTrebleGain(float newTrebleGain);
    void setOutputLevel(float newDB);

    
private:
    float applySat(float inputSample);
    float sampleRate = 44100.0f;
    const float saturationBias = 0.25f;

    juce::dsp::Gain<float> inputGain;
    juce::dsp::Gain<float> outputGain;

    

    using FilterType = juce::dsp::IIR::Filter<float>;
    using CoeffsType = juce::dsp::IIR::Coefficients<float>;

    juce::dsp::ProcessorDuplicator<FilterType, CoeffsType> bassFilter;
    juce::dsp::ProcessorDuplicator<FilterType, CoeffsType> trebleFilter;

    juce::dsp::ProcessorDuplicator<FilterType, CoeffsType> dcBlocker;
};