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

    //kalapácsütés
    hammerModule.prepareHammer(getSampleRate());
    hammerModule.triggerHammer(velocity);

    noteCurrentlyActive = true;
}

void DWMVoice::stopNote(float velocity, bool tailOffAllowed) {
    
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

        //0.997f átmeneti érték - sustainért felel
        float soundSample = dlModule.processSample(hammerSample, 0.997f);

        //csatornánként hozzáadjuk a sample-t
        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel) {
            outputBuffer.addSample(channel, startSample + sample, soundSample * 0.2f);

        }

        //ha már leállt a kalapács és a delayline nagyon alacsony értéket ad, leállítjuk
        if (!hammerModule.isHammerActive() && std::abs(soundSample) < 0.0001f) {
            noteCurrentlyActive = false;
            clearCurrentNote();
            break;
        }

    }
}