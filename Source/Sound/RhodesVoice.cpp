/*
  ==============================================================================

    RhodesVoice.cpp
    Created: 24 Dec 2025 3:43:06pm
    Author:  kadar

  ==============================================================================
*/

#include "RhodesVoice.h"

//ez az egyik fo hanggeneralo modul amely a legtobb modul osszekoti

void RhodesVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) {

    float velocityCurve = std::pow(velocity, 2.0f);
    currentVelocity = velocity;

    //frekvencia kinyerese a midi hangbol
    float frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber, 440.0);

    juce::Random keyRNG(midiNoteNumber * 12345); //hangonkent fix seed
    float detuneRand = 1.0f + (keyRNG.nextFloat() * 0.001f - 0.0005f); //detune
    currentFrequency = frequency * detuneRand;


    //travel time
    float delaySecs = 0.002f + (0.005f * (1.0f - (static_cast<float>(midiNoteNumber) / 112.0f)));
    float delaySamples = delaySecs * getSampleRate();


    //tine params
    float decayTime = juce::jmap((float)midiNoteNumber, 21.0f, 108.0f, 3.5f, 0.4f);
    float toneBrightness = velocity;
    modalTine.setParams(currentFrequency, decayTime * decayMultiplier, toneBrightness);

    //kemenyseg
    float stiffnessBase = 200000000.0f;
    float stiffnessMultiplier = std::pow(1.2f, (midiNoteNumber) - 60.0f) * hammerHardness;
    float currentStiffness = stiffnessBase * stiffnessMultiplier;

    float massBase = 0.006f;
    float currentMass = massBase / (1.0f + (midiNoteNumber / 60.0f));//felfele konnyeb 
    if (currentMass < 0.001f) currentMass = 0.001f;

    hammer.setParameters(currentStiffness, currentMass);
    hammer.triggerHammer(currentVelocity, delaySamples, midiNoteNumber);
    
    pickup.setFrequency(currentFrequency);

    previousTinePos = 0.0f;
    prevIn = 0.0f;
    prevOut = 0.0f;
    fadeoutGain = 1.0f;

    noteCurrentlyActive = true;
    isKeyHeld = true;
    damping = false;
    isFading = false;
}

void RhodesVoice::stopNote(float velocity, bool tailOffAllowed) { 
    if (tailOffAllowed) {
        damping = true;
        isKeyHeld = false;

        float dampFactor = (1.0f - releaseTime) * 5.0f;
        modalTine.applyDamping(dampFactor);
    }
    else {
        isKeyHeld = false;
        isFading = true;
    }
}

//tenyleges hanggeneralas, a kulonbozo modulok egybefonodasa itt tortenik
void RhodesVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSample) {

    if (!noteCurrentlyActive) return;

    auto* left = outputBuffer.getWritePointer(0); // csatornak szetvalasztasa
    auto* right = (outputBuffer.getNumChannels() > 1) ? outputBuffer.getWritePointer(1) : nullptr;

    for (int sample = 0; sample < numSample; ++sample) {

        float tinePos = modalTine.getCurrentPos();

        //kalapacs utes
        float hammerForce = hammer.getNextSample(tinePos);
        if(std::isnan(hammerForce)) hammerForce = 0.0f;

        float invDistance = 1.0f - pickupDistance;
        float distanceGain = 1.0f + (invDistance * 8.0f);

        //tine
        float tineVelocity = modalTine.process(hammerForce);

        float thumpSignal = hammerForce * 0.7f;
        float mixedSignal = tineVelocity + thumpSignal;

        //erosites
        float monoSample = mixedSignal * distanceGain * outputGain;

        //hangszedo
        float pickupSignal = pickup.processSample(monoSample);
        float comp = 1.0f / (distanceGain * 0.8f + 0.2f);
        pickupSignal *= comp;

        if (isFading) {
            fadeoutGain *= 0.9f;
            pickupSignal *= fadeoutGain;
            if (fadeoutGain < 0.001f) {
                clearCurrentNote();
                noteCurrentlyActive = false;
                break;
            }
        }

        Stereo stereoOutput;
        //tremolo
        if (tremolo.isTremoloEnabled()) {
            stereoOutput = tremolo.process(pickupSignal);
        }
        else {
            stereoOutput.left = pickupSignal;
            stereoOutput.right = pickupSignal;
        }

        //csatornankent output es hangero beallitasa
        int index = startSample + sample;
        left[index] += stereoOutput.left * voiceVolume;
        if (right) right[index] += stereoOutput.right * voiceVolume;
        
        //hang leallitasi feltetelek
        if (!hammer.isHammerActive() && !isKeyHeld && std::abs(pickupSignal) < 0.0001f && std::abs(monoSample) < 0.0001f) {
            noteCurrentlyActive = false;
            clearCurrentNote();
            break;
        }

    }
}

void RhodesVoice::prepare(const juce::dsp::ProcessSpec& specs) {

    modalTine.prepare(specs);
    hammer.prepareHammer(specs);

    pickup.prepare(specs);
    pickup.setParameters(14.0f, 6.0f, 12000.0f);

    tremolo.prepare(specs.sampleRate);
    tremolo.setTremRate(1.4f);
    tremolo.setDepth(0.8f);
}