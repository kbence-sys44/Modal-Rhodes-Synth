/*
  ==============================================================================

    DWMVoice.h
    Created: 24 Dec 2025 3:43:06pm
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "HammerModule.h"
#include "DelayLineModule.h"
#include "ToneBarModule.h"
#include "PickupModule.h"
#include "Tremolo.h"
#include "Preamp.h"
#include "ModalTine.h"


class DWMVoice : public juce::SynthesiserVoice
{
public:
    DWMVoice() {}

    void prepare(const juce::dsp::ProcessSpec& specs);

    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool tailOffAllowed) override;

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSample) override;

    bool canPlaySound(juce::SynthesiserSound* sound) override { return true; }
    void pitchWheelMoved(int newPitchWheelValue) override {};
    void controllerMoved(int controllerNumber, int newControllerValue) override {};

    float addDamping(float inputSample);

private:
    struct ForkMix {
        float tineMix;
        float tonebarMix;
    };
    ForkMix DWMVoice::calculateForkMix(float frequency, float velocity);
    ForkMix currentMix;

    DelayLineModule dlModule;
    ModalTine modalTine;
    HammerModule hammerModule;
    ToneBarModule toneBarModule;
    PickupModule pickupModule;
    Tremolo tremolo;
    Preamp preamp;
    

    float tonebarMix = 0.5f;
    float currentFrequency = 0.0f;
    float lastHammer = 0.0f;
    float voiceVolume = 0.5f;
    float thumpLevel = 0.5f;
    float currentVelocity = 0.0f;

    bool noteCurrentlyActive = false;
    bool isKeyHeld = false;
    bool triggerThump = true;

    juce::dsp::IIR::Filter<float> dcBlocker;
    juce::Random random;

    //Damper
    bool damperActive = false;
    float damperNoiseLevel = 0.0f;
    float damperEnv = 0.0f;
    float damperDecay = 0.95f;

    juce::Random damperRand;
    juce::dsp::FirstOrderTPTFilter<float> damperFilter;

    JUCE_LEAK_DETECTOR(DWMVoice)
};