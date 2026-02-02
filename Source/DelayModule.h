/*
  ==============================================================================

    DelayModule.h
    Created: 2 Feb 2026 4:55:39pm
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class DelayModule
{
public:
    DelayModule() {
        delayLine.setMaximumDelayInSamples(44100 * maxDelayInSeconds);
    }
    ~DelayModule() = default;

    void prepare(const juce::dsp::ProcessSpec& specs);
    void reset();

    void setParameters(float timeInMs, float feedback, float mixRatio, float toneHz);
    void process(juce::dsp::ProcessContextReplacing<float>& context);

private:
    float sampleRate = 44100.0f;

    static constexpr float maxDelayInSeconds = 2.0f;

    //interpolacio
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLine;

    std::array<juce::dsp::FirstOrderTPTFilter<float>, 2> filters; //csatornankent 1 filter

    juce::SmoothedValue<float> delayTimeSmooth;
    juce::SmoothedValue<float> feedbackSmooth;
    juce::SmoothedValue<float> mixSmooth;

    


};