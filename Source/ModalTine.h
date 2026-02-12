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

//minden modus adatait ez tarolja
struct Modals {
    float displacement = 0.0f;
    float velocity = 0.0f;
    float omega = 0.0f;
    float decay = 0.0f;
    float mass = 1.0f;
    float gain = 1.0f;
};

class ModalTine {
public:
    ModalTine();
    ~ModalTine();

    void prepare(const juce::dsp::ProcessSpec& specs);
    void reset();

    void setParams(float baseFrequency, float decayTimeSecs, float toneKnob);
    float process(float inputForce);

    float getCurrentPos() const { return displacement; };

    void applyDamping(float release);

private:
    double sampleRate = 44100.0;
    float dt = 1.0f / 44100.0f;
    float globalVolumeAdjust = 1.0f;
    float displacement = 0.0f;
    float outputVel = 0.0f;
    
    std::vector<Modals> modes;    

    void setupMode(float frequency, float decayMult, float tone);

    JUCE_LEAK_DETECTOR(ModalTine)
};