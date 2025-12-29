/*
  ==============================================================================

    DWMVoice.cpp
    Created: 24 Dec 2025 3:43:06pm
    Author:  kadar

  ==============================================================================
*/

#include "DWMVoice.h"

void DWMVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) {

    //frekvencia kinyerése a midi hangból
    float frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);

    //delay kiszámolása (húr hossztól függ)
    float delaySamples = static_cast<float>(getSampleRate() / frequency);
    //delay átadása a delayline modulnak
    dlModule.setDelayForDelayLine(delaySamples);

    noteCurrentlyActive = true;
    isKeyHeld = true;

    //kalapácsütés
    hammerModule.triggerHammer(velocity);
    
}

void DWMVoice::stopNote(float velocity, bool tailOffAllowed) {
    isKeyHeld = false;

    if (!tailOffAllowed) {
        noteCurrentlyActive = false;
        clearCurrentNote();

    }
}

//tényleges hanggenerálás
void DWMVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSample) {

    if (!noteCurrentlyActive) return;

    for (int sample = 0; sample < numSample; ++sample) {
        
        float hammerSample = hammerModule.getNextSample();

        //alapértelmezett sustain;
        float feedback = 0.999f;

        //természetes elhalás
        if (!isKeyHeld) {
            feedback = 0.7f;
        }

        float soundSample = dlModule.processSample(hammerSample, feedback);

        //csatornánként hozzáadjuk a sample-t
        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel) {
            outputBuffer.addSample(channel, startSample + sample, soundSample * 0.2f);

        }

        //ha már leállt a kalapács és a delayline nagyon alacsony értéket ad, leállítjuk
        if (!hammerModule.isHammerActive() && !isKeyHeld && std::abs(soundSample) < 0.00001f) {
            noteCurrentlyActive = false;
            clearCurrentNote();
            break;
        }

    }
}

void DWMVoice::prepare(const juce::dsp::ProcessSpec& specs) {

    dlModule.prepareDelayLine(specs);

    hammerModule.prepareHammer(specs.sampleRate);
}