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

    float pickupDelay = 14.0f;
    if (frequency < 200.0f) {
        pickupDelay = 10.0f + random.nextFloat() * 8.0f;
    }
    else {
        pickupDelay = 6.0f + random.nextFloat() * 4.0f;
    }

    pickupModule.setBaseDelay(pickupDelay);

    float detune = 1.0f + ((juce::Random::getSystemRandom().nextFloat() * 0.001f) - 0.0005f);
    float detunedFrequency = frequency * detune;

    currentFrequency = detunedFrequency;

    float bassGain = 1.5f;

    if (currentFrequency > 400.0f) {
        bassGain = 1.0f;
    }
    pickupModule.setBassGain(bassGain);

    lastHammer = 0.0f;
    if (frequency < 200.0f) {
        tonebarMix = 0.4f;
    }
    else if (frequency < 500.0f) {
        tonebarMix = juce::jmap(frequency, 200.0f, 2000.0f, 0.4f, 0.2f);
    }
    else {
        tonebarMix = 0.15f;
    }

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

        if (currentFrequency > 600.0f) {
            float filteredHammer = hammerSample - (lastHammer * 0.95f);
            lastHammer = hammerSample;
            hammerSample = filteredHammer * 2.5f;
        }

        float toneBarSample = toneBarModule.getNextSample();

        float feedback = 0.999f;
        if (currentFrequency > 100.0f) {
            feedback = 0.999f - (currentFrequency * 0.000010f);
        }
        feedback = juce::jmax(0.985f, feedback);
        float finalFeedback = isKeyHeld ? feedback : 0.8f;


        float tineSample = dlModule.processSample(hammerSample, feedback);
        float tineMix = 1.0f - tonebarMix;
        float rawSample = (tineSample * tineMix) - (toneBarSample * tonebarMix);

        float pickupSample = pickupModule.processSignal(rawSample);
        Stereo stereoOutput = tremolo.process(pickupSample);

        float cleanLeft = dcBlocker.processSample(stereoOutput.left);
        float cleanRight = dcBlocker.processSample(stereoOutput.right);

        if (outputBuffer.getNumChannels() >= 1) {
            outputBuffer.addSample(0, startSample + sample, cleanLeft * voiceVolume);
        }
        if (outputBuffer.getNumChannels() >= 1) {
            outputBuffer.addSample(1, startSample + sample, cleanRight * voiceVolume);
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

    dcBlocker.prepare(specs);
    dcBlocker.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(specs.sampleRate, 40.0f);
}