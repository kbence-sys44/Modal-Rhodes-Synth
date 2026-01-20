/*
  ==============================================================================

    ModalTine.h
    Created: 18 Jan 2026 3:14:54pm
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class ModalTine {
public:
    ModalTine() = default;
    ~ModalTine() = default;

    void prepare(const juce::dsp::ProcessSpec& specs);
    void reset();

    void triggerTine(float frequency, float velocity);
    float processSample(float inputSample);

    void damp();

private:
    double sampleRate = 44100.0;
    
    struct ModalFilter {
        juce::dsp::StateVariableTPTFilter<float> filter;
        float gain = 0.0f;
        float frequencyRatio = 1.0f;
        float decayTime = 1.0f;

        void prepare(const juce::dsp::ProcessSpec& specs) {
            filter.prepare(specs);
            filter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
        }

        void reset() {
            filter.reset();
        }
    };

    std::array<ModalFilter, 6> modes;

    float calculateQ(float frequency, float decayTime); //Q = pi * freq * decay

    JUCE_LEAK_DETECTOR(ModalTine)
};