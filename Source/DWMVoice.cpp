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

    float velocityCurve = std::pow(velocity, 3.0f);
    currentVelocity = velocityCurve;

    //frekvencia kinyerese a midi hangbol
    float frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber, 440.0);
    float distanceFromCenter = (float)midiNoteNumber - 60.0;
    float stretch = 0.0005 * distanceFromCenter * std::abs(distanceFromCenter);//szetcsusztatja a frekvenciakat
    float stretchedFrequency = frequency * std::pow(2.0f, stretch / 12.0f); //f = f0 * 2^semitones / 12

    currentMix = calculateForkMix(frequency, currentVelocity);

    juce::Random keyRNG(midiNoteNumber + 42); //hangonkent fix seed

    float hammerRand = 1.0f + (keyRNG.nextFloat() * 0.1f - 0.05f); //kalapacs kemenyseg
    float detuneRand = 1.0f + (keyRNG.nextFloat() * 0.002f - 0.001f); //detune

    currentFrequency = stretchedFrequency * detuneRand;

    float pickupDelay = 14.0f;
    if (frequency < 200.0f) {
        pickupDelay = 10.0f + random.nextFloat() * 8.0f;
    }
    else {
        pickupDelay = 6.0f + random.nextFloat() * 4.0f;
    }

    pickupModule.setBaseDelay(pickupDelay);


    float bassGain = 1.5f;

    if (currentFrequency > 400.0f) {
        bassGain = 1.0f;
    }
    pickupModule.setBassGain(bassGain);

    lastHammer = 0.0f;

    //delay kiszamolasa (hur hossztol fugg)
    float delaySamples = static_cast<float>(getSampleRate() / currentFrequency);
    dlModule.setDelayForDelayLine(delaySamples, currentVelocity, currentFrequency);

    noteCurrentlyActive = true;
    isKeyHeld = true;
    triggerThump = true;
    thumpLevel = currentVelocity * 0.5f;

    
    hammerModule.triggerHammer(currentVelocity * hammerRand, delaySamples);
    toneBarModule.triggerToneBar(currentFrequency, currentVelocity);


}

void DWMVoice::stopNote(float velocity, bool tailOffAllowed) {
    isKeyHeld = false;
    toneBarModule.releaseToneBar();

    if (tailOffAllowed) {
        damperActive = true;
        damperEnv = 1.0f;
        damperNoiseLevel = currentVelocity * 0.3f;
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

    for (int sample = 0; sample < numSample; ++sample) {
        
        float hammerSample = hammerModule.getNextSample();

        if (currentFrequency > 600.0f) {
            float filteredHammer = hammerSample - (lastHammer * 0.95f);
            lastHammer = hammerSample;
            hammerSample = filteredHammer * 2.5f;
        }

        float hammerInput = hammerSample * 2.5f;
        float toneBarSample = toneBarModule.getNextSample();

        float feedback = 0.999f;
        if (currentFrequency > 100.0f) {
            feedback = 0.999f - (currentFrequency * 0.000010f);
        }
        feedback = juce::jmax(0.985f, feedback);
        float finalFeedback = isKeyHeld ? feedback : 0.8f;

        float tineSample = dlModule.processSample(hammerInput, finalFeedback);
        float rawSample = (tineSample * currentMix.tineMix) - (toneBarSample * currentMix.tonebarMix);

        float pickupSample = pickupModule.processSignal(rawSample);

        float ampedSample = preamp.processSample(pickupSample);

        float dampSample = addDamping(ampedSample);

        Stereo stereoOutput = tremolo.process(dampSample);

        float cleanLeft = dcBlocker.processSample(stereoOutput.left);
        float cleanRight = dcBlocker.processSample(stereoOutput.right);

        if (outputBuffer.getNumChannels() >= 1) {
            outputBuffer.addSample(0, startSample + sample, cleanLeft * voiceVolume);
        }
        if (outputBuffer.getNumChannels() >= 1) {
            outputBuffer.addSample(1, startSample + sample, cleanRight * voiceVolume);
        }

        //hang leallitasi feltetelek
        if (!hammerModule.isHammerActive() && !toneBarModule.isToneBarActive() && std::abs(pickupSample) < 0.00001f && !isKeyHeld) {
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

    preamp.prepare(specs);
    preamp.setDrive(4.0f);

    tremolo.prepare(specs.sampleRate);
    tremolo.setTremRate(1.5f);
    tremolo.setDepth(0.7f);

    dcBlocker.prepare(specs);
    dcBlocker.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(specs.sampleRate, 10.0f);

    damperFilter.prepare(specs);
    damperFilter.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
    damperFilter.setCutoffFrequency(150.0f);
}

//a tine es a tonebar aranya frekvenciafuggo, logaritmikus szamolassal pontosabb, mint a fix ertekek
DWMVoice::ForkMix DWMVoice::calculateForkMix(float frequency, float velocity) {
    ForkMix coeffs;

    float frequencyLog = std::log10(frequency);
    float minLog = std::log10(40.0f); //E1
    float maxLog = std::log10(3000.0f); //G7

    float normalizedFrequency = juce::jlimit(0.0f, 1.0f, (frequencyLog - minLog) / (maxLog - minLog));

    float defaultTineMix = juce::jmap(normalizedFrequency, 0.25f, 1.0f);
    float defaultTonebarMix = juce::jmap(normalizedFrequency, 1.0f, 0.25f);

    //velocity is befolyasolja a jo mixet

    float velocityFactor = velocity * velocity;
    coeffs.tineMix = defaultTineMix * (0.6f + (0.8f * velocityFactor));
    coeffs.tonebarMix = defaultTonebarMix * (0.8f + (0.4f * velocity));
    //DBG(defaultTineMix);
    //DBG(defaultTonebarMix);
    
    return coeffs;
}

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