/*
  ==============================================================================

    DWMVoice.cpp
    Created: 24 Dec 2025 3:43:06pm
    Author:  kadar

  ==============================================================================
*/

#include "DWMVoice.h"

//ez az egyik fo hanggeneralo modul amely a tobbit osszekoti

void DWMVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) {
    //frekvencia kinyerese a midi hangbol
    float frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);

    float detune = 1.0f + ((juce::Random::getSystemRandom().nextFloat() * 0.001f) - 0.0005f);
    float detunedFrequency = frequency * detune;

    //delay kiszamolasa (hur hossztol fugg)
    float delaySamples = static_cast<float>(getSampleRate() / detunedFrequency);
    dlModule.setDelayForDelayLine(delaySamples, velocity);

    noteCurrentlyActive = true;
    isKeyHeld = true;

    hammerModule.triggerHammer(velocity, delaySamples);


    toneBarModule.triggerToneBar(detunedFrequency, velocity);
    
}

void DWMVoice::stopNote(float velocity, bool tailOffAllowed) {
    isKeyHeld = false;
    toneBarModule.releaseToneBar();

    if (!tailOffAllowed) {
        noteCurrentlyActive = false;
        clearCurrentNote();

    }
}

//tenyleges hanggeneralas, a kulonbozo modulok egybefonodasa itt tortenik
void DWMVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSample) {

    if (!noteCurrentlyActive) return;

    for (int sample = 0; sample < numSample; ++sample) {
        
        float hammerSample = hammerModule.getNextSample();

        float toneBarSample = toneBarModule.getNextSample();

        float feedback = isKeyHeld ? 0.999f : 0.8f;
        float tineSample = dlModule.processSample(hammerSample, feedback);

        float rawSample = (tineSample * 0.9f) - (toneBarSample * 0.2f);

        float pickupSample = pickupModule.processSignal(rawSample);

        Stereo stereoOutput = tremolo.process(pickupSample);

        if (outputBuffer.getNumChannels() >= 1) {
            outputBuffer.addSample(0, startSample + sample, stereoOutput.left * 0.5f);
        }
        if (outputBuffer.getNumChannels() >= 1) {
            outputBuffer.addSample(1, startSample + sample, stereoOutput.right * 0.5f);
        }

        //hang leallitasi feltetelek
        if (!hammerModule.isHammerActive() && !toneBarModule.isToneBarActive() && !isKeyHeld && std::abs(pickupSample) < 0.00001f) {
            noteCurrentlyActive = false;
            clearCurrentNote();
            break;
        }

    }
}

void DWMVoice::prepare(const juce::dsp::ProcessSpec& specs) {

    dlModule.prepareDelayLine(specs);

    hammerModule.prepareHammer(specs.sampleRate);

    toneBarModule.prepareToneBar(specs.sampleRate);

    pickupModule.preparePickup(specs);

    tremolo.prepare(specs.sampleRate);
    tremolo.setTremRate(6.0f);
    tremolo.setDepth(0.7f);
}