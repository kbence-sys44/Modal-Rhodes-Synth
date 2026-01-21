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

    float velocityCurve = std::pow(velocity, 2.0f);
    currentVelocity = velocity;

    //frekvencia kinyerese a midi hangbol
    float frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber, 440.0);

    juce::Random keyRNG(midiNoteNumber + 42); //hangonkent fix seed
    float detuneRand = 1.0f + (keyRNG.nextFloat() * 0.002f - 0.001f); //detune
    currentFrequency = frequency * detuneRand;


    //travel time
    float delaySecs = 0.002f + (0.005f * (1.0f - (static_cast<float>(midiNoteNumber) / 127.0f)));
    float delaySamples = delaySecs / getSampleRate();

   //float bassGain = juce::jmap(currentFrequency, 50.0f, 1000.0f, 1.5f, 1.0f);
   // pickupModule.setBassGain(bassGain);

    float stiffnessBase = 200000000.0f;
    float stiffnessMultiplier = std::pow(2.0f, (midiNoteNumber / 12.0f));
    float currentStiffness = stiffnessBase * stiffnessMultiplier;

    float massBase = 0.002f;
    float currentMass = massBase / (1.0f + (midiNoteNumber / 48.0f));

    hammer.setParameters(currentStiffness, currentMass);

    modalTine.triggerTine(currentFrequency, currentVelocity);
    hammer.triggerHammer(currentVelocity, delaySamples, midiNoteNumber);
    pickup.setFrequency(currentFrequency);

    noteCurrentlyActive = true;
    isKeyHeld = true;

    tonebar.reset();
    //triggerThump = true;
}

void DWMVoice::stopNote(float velocity, bool tailOffAllowed) {
    isKeyHeld = false;
    //toneBarModule.releaseToneBar();

    if (tailOffAllowed) {
        damperActive = true;
        damperEnv = 1.0f;
        damperNoiseLevel = currentVelocity * 0.1f;

        modalTine.damp();
    }
    else {
        noteCurrentlyActive = false;
        damperActive = false;
        clearCurrentNote();
    }
}

//tenyleges hanggeneralas, a kulonbozo modulok egybefonodasa itt tortenik
void DWMVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSample) {

    if (!noteCurrentlyActive) return;

    auto* left = outputBuffer.getWritePointer(0);
    auto* right = (outputBuffer.getNumChannels() > 1) ? outputBuffer.getWritePointer(1) : nullptr;

    for (int sample = 0; sample < numSample; ++sample) {
        
        float hammerForce = hammer.getNextSample();
        float hammerThump = hammer.getThump();

        float tineSignal = modalTine.processSample(hammerForce);

        float tbInput = (hammerForce * 0.2f) + (hammerThump * 0.1f);
        float bodySignal = tonebar.processSample(tbInput);

        float rawSignal = tineSignal + bodySignal;

        float pickupSignal = pickup.processSample(rawSignal);

        float outputSignal = pickupSignal *= voiceVolume;

        int index = startSample + sample;
        left[index] += outputSignal;
        if (right) right[index] += outputSignal;


        //float pickupSample = pickupModule.processSignal(rawInputForPickup);

        //float dampSample = addDamping();

        //Stereo stereoOutput = tremolo.process(dampSample);

        //float cleanLeft = dcBlocker.processSample(stereoOutput.left);
        //float cleanRight = dcBlocker.processSample(stereoOutput.right);
        
        //hang leallitasi feltetelek && std::abs(pickupSample) < 0.00001f
        if (!hammer.isHammerActive() && !isKeyHeld && std::abs(outputSignal < 0.0001f)) {
            noteCurrentlyActive = false;
            clearCurrentNote();
            break;
        }

    }
}

void DWMVoice::prepare(const juce::dsp::ProcessSpec& specs) {

    modalTine.prepare(specs);
    hammer.prepareHammer(specs);
    pickup.prepare(specs);

    pickup.setParameters(10.0f, 4.0f, 5000.0f);

    //pickupModule.preparePickup(specs);

    //preamp.prepare(specs);
    //preamp.setDrive(1.2f);

    //tremolo.prepare(specs.sampleRate);
    //tremolo.setTremRate(1.5f);
    //tremolo.setDepth(0.7f);

    //dcBlocker.prepare(specs);
    //dcBlocker.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(specs.sampleRate, 10.0f);

    //damperFilter.prepare(specs);
    //damperFilter.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
    //damperFilter.setCutoffFrequency(150.0f);
}

/*
DWMVoice::ForkMix DWMVoice::calculateForkMix(float frequency, float velocity) {
    ForkMix coeffs;
    coeffs.tineMix = 1.0f;
    coeffs.tonebarMix = 1.0f;

    if (frequency < 200.0f) {
        coeffs.tonebarMix = 0.8f;
        coeffs.tineMix = 1.0f + (velocity * 0.4f);
    }
    else if (frequency < 1000.0f) {
        coeffs.tonebarMix = 0.4f;
        coeffs.tineMix = 1.2f;
    }
    else {
        coeffs.tonebarMix = 0.6f;
        coeffs.tineMix = 1.0f;
    }
    
    return coeffs;
}*/

float DWMVoice::addDamping(float inputSample) {
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

}