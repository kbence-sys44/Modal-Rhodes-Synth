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

    float detune = (juce::Random::getSystemRandom().nextFloat() * 1.0f) - 0.5f;
    float detunedFrequency = frequency + detune;

    //delay kiszámolása (húr hossztól függ)
    float delaySamples = static_cast<float>(getSampleRate() / detunedFrequency);
    //delay átadása a delayline modulnak
    dlModule.setDelayForDelayLine(delaySamples);

    noteCurrentlyActive = true;
    isKeyHeld = true;

    //kalapácsütés
    hammerModule.triggerHammer(velocity);

    //tonebar rezegtetés indítása
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

//tényleges hanggenerálás
void DWMVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSample) {

    if (!noteCurrentlyActive) return;

    for (int sample = 0; sample < numSample; ++sample) {
        
        float hammerSample = hammerModule.getNextSample();

        float toneBarSample = toneBarModule.getNextSample();

        //alapértelmezett sustain;
        float feedback = isKeyHeld ? 0.999f : 0.8f;
        float tineSample = dlModule.processSample(hammerSample, feedback);

        //arányok nem véglegesek
        float rawSample = (tineSample * 0.4f) + (toneBarSample * 0.6f);

        //pickup, ide majd kell még szűrés
        float pickupSample = rawSample;

        //szaturáció
        float drive = 2.5f;
        float saturatedSample = std::tanh(pickupSample * drive);

        //a szaturáció hangosít, ezért kompenzálni kell
        saturatedSample *= 0.7f;

        //csatornánként hozzáadjuk a sample-t
        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel) {
            outputBuffer.addSample(channel, startSample + sample, saturatedSample * 0.2f);

        }

        //ha már leállt a kalapács és a delayline nagyon alacsony értéket ad, leállítjuk
        if (!hammerModule.isHammerActive() && !toneBarModule.isToneBarActive() && !isKeyHeld && std::abs(rawSample) < 0.00001f) {
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
}