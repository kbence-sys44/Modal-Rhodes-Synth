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
    float delaySecs = 0.002f + (0.005f * (1.0f - (static_cast<float>(midiNoteNumber) / 127.0f)));
    float delaySamples = delaySecs * getSampleRate();


    //tine params
    float decayTime = juce::jmap((float)midiNoteNumber, 21.0f, 108.0f, 3.5f, 0.4f);
    float toneBrightness = velocity;
    modalTine.setParams(currentFrequency, decayTime, toneBrightness);

    //kemenyseg
    float stiffnessBase = 200000000.0f;
    float stiffnessMultiplier = std::pow(1.12f, (midiNoteNumber) - 60.0f);
    float currentStiffness = stiffnessBase * stiffnessMultiplier;

    float massBase = 0.006f;
    float currentMass = massBase / (1.0f + (midiNoteNumber / 60.0f));//felfele konnyeb 
    if (currentMass < 0.001f) currentMass = 0.001f;

    hammer.setParameters(currentStiffness, currentMass);
    hammer.triggerHammer(currentVelocity, delaySamples, midiNoteNumber);
    
    pickup.setFrequency(currentFrequency);

    float trebleBoost = std::pow(1.03f, midiNoteNumber);
    baseGain = 5000.0f;
    outputGain = baseGain * trebleBoost;

    previousTinePos = 0.0f;
    prevIn = 0.0f;
    prevOut = 0.0f;

    noteCurrentlyActive = true;
    isKeyHeld = true;

}

void RhodesVoice::stopNote(float velocity, bool tailOffAllowed) {
    isKeyHeld = false;

    if (!tailOffAllowed) {
        noteCurrentlyActive = false;
        clearCurrentNote();
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
        float hammerThump = hammer.getThump();

        //tine
        float tineDisplacement = modalTine.process(hammerForce);
        float velocitySignal = tineDisplacement - previousTinePos; //poziciobol sebesseg szamitasa
        previousTinePos = tineDisplacement;
        float filtered = velocitySignal - prevIn + (0.995f * prevOut);

        if (std::abs(filtered) < 1.0e-8f) filtered = 0.0f;

        prevIn = velocitySignal;
        prevOut = filtered;

        //erosites
        float monoSample = filtered * outputGain;

        //koppanas
        float rawSignal = monoSample + hammerThump; //+ (bodySignal* 0.1f); 

        //hangszedo
        float pickupSignal = pickup.processSample(rawSignal);

        //tremolo
        Stereo tremoloOutput = tremolo.process(pickupSignal);

        //csatornankent output es hangero beallitasa
        int index = startSample + sample;
        left[index] += tremoloOutput.left * voiceVolume;
        if (right) right[index] += tremoloOutput.right * voiceVolume;
        
        //hang leallitasi feltetelek
        if (!hammer.isHammerActive() && !isKeyHeld && std::abs(pickupSignal < 0.000001f) && std::abs(monoSample) < 0.00001f) {
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
    pickup.setParameters(9.0f, 6.0f, 6000.0f);

    tremolo.prepare(specs.sampleRate);
    tremolo.setTremRate(1.4f);
    tremolo.setDepth(0.8f);
}

/*float RhodesVoice::addDamping(float inputSample) {
    float output = inputSample;
    if (damperActive) {
        float noise = (damperRand.nextFloat() * 2.0f) - 1.0f;
        float thud = damperFilter.processSample(0, noise);
        float damperSignal = thud * damperEnv * damperNoiseLevel;

        output += damperSignal;
        damperEnv *= damperDecay;

        if (damperEnv < 0.001f) {
            damperActive = false;
        }
    }

    return output;

}*/