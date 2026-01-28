/*
  ==============================================================================

    ModalTine.h
    Created: 18 Jan 2026 3:14:54pm
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <vector>

struct Modals {
    //juce::dsp::StateVariableTPTFilter<float> filter;
    float displacement = 0.0f;
    float velocity = 0.0f;

    float omega = 0.0f;
    float decay = 0.0f;
    float mass = 1.0f;
    float gain = 1.0f;

    //float gain = 0.0f;
    //float frequencyRatio = 1.0f;
    //float decayTime = 1.0f;
    /*
    void prepare(const juce::dsp::ProcessSpec& specs) {
        filter.prepare(specs);
        filter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    }

    void reset() {
        filter.reset();
    }*/
};

class ModalTine {
public:
    ModalTine();
    ~ModalTine();

    void prepare(const juce::dsp::ProcessSpec& specs);
    void reset();

    void setParams(float baseFrequency, float decayTimeSecs, float toneKnob);
    float process(float inputForce);

    float getCurrentPos() const { return displacement; }

private:
    double sampleRate = 44100.0;
    float dt = 1.0f / 44100.0f;
    float globalVolumeAdjust = 1.0f;
    float displacement = 0.0f;

    //juce::dsp::Oversampling<float> oversampling { (size_t) 2, (size_t) 1, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR };
    //juce::AudioBuffer<float> tempBuffer;
    
    std::vector<Modals> modes;    

    void setupMode(float frequency, float decayMult, float tone);

    //std::array<ModalFilter, 17> modes;

    //float calculateQ(float frequency, float decayTime); //Q = pi * freq * decay
    //void configSpikes(int index, float frequency, float decay, float gain, float QMulti);

    JUCE_LEAK_DETECTOR(ModalTine)
};