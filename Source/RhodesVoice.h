/*
  ==============================================================================

    RhodesVoice.h
    Created: 24 Dec 2025 3:43:06pm
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PickupModule.h"
#include "Tremolo.h"
#include "ModalTine.h"
#include "CrossleyHammer.h"


class RhodesVoice : public juce::SynthesiserVoice
{
public:
    RhodesVoice() = default;

    void prepare(const juce::dsp::ProcessSpec& specs);

    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool tailOffAllowed) override;

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSample) override;

    bool canPlaySound(juce::SynthesiserSound* sound) override { return true; }
    void pitchWheelMoved(int newPitchWheelValue) override {};
    void controllerMoved(int controllerNumber, int newControllerValue) override {};

    float addDamping(float inputSample);

private:
    ModalTine modalTine;
    CrossleyHammer hammer;
    PickupModule pickup;
    Tremolo tremolo;
    
    float baseGain = 5000.0f;
    float currentFrequency = 0.0f;
    float voiceVolume = 1.0f;
    float currentVelocity = 0.0f;
    float outputGain = 1.0f;

    float previousTinePos = 0.0f;

    bool noteCurrentlyActive = false;
    bool isKeyHeld = false;
    bool triggerThump = true;

    float prevIn = 0.0f;
    float prevOut = 0.0f;

    juce::dsp::IIR::Filter<float> dcBlocker;
    juce::Random random;

    JUCE_LEAK_DETECTOR(RhodesVoice)
};