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
    float frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber, 440.0);
    currentMix = calculateForkMix(frequency, velocity);

    float pickupDelay = 14.0f;
    if (frequency < 200.0f) {
        pickupDelay = 10.0f + random.nextFloat() * 8.0f;
    }
    else {
        pickupDelay = 6.0f + random.nextFloat() * 4.0f;
    }

    pickupModule.setBaseDelay(pickupDelay);

    float detune = 1.0f + ((juce::Random::getSystemRandom().nextFloat() * 0.0002f) - 0.0001f);
    float detunedFrequency = frequency * detune;

    currentFrequency = detunedFrequency;

    float bassGain = 1.5f;

    if (currentFrequency > 400.0f) {
        bassGain = 1.0f;
    }
    pickupModule.setBassGain(bassGain);

    lastHammer = 0.0f;

    //delay kiszamolasa (hur hossztol fugg)
    float delaySamples = static_cast<float>(getSampleRate() / detunedFrequency);
    dlModule.setDelayForDelayLine(delaySamples, velocity);

    noteCurrentlyActive = true;
    isKeyHeld = true;
    triggerThump = true;
    thumpLevel = velocity * 0.5f;

    
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

        float thump = 0.0f;
        //duborges generalas
        if (triggerThump && thumpLevel > 0.001f) {
            float noise = (random.nextFloat() * 2.0f - 1.0f);
            thump = thumpFilter.processSample(noise * thumpLevel);
            //lecsenges
            thumpLevel *= 0.95f;
        }

        float inputForPickup = rawSample + (thump * 0.8f);

        float pickupSample = pickupModule.processSignal(inputForPickup);

        //float ampedSample = preamp.processSample(pickupSample);

        Stereo stereoOutput = tremolo.process(tineSample);

        float cleanLeft = dcBlocker.processSample(stereoOutput.left);
        float cleanRight = dcBlocker.processSample(stereoOutput.right);

        if (outputBuffer.getNumChannels() >= 1) {
            outputBuffer.addSample(0, startSample + sample, rawSample * voiceVolume);
        }
        if (outputBuffer.getNumChannels() >= 1) {
            outputBuffer.addSample(1, startSample + sample, rawSample * voiceVolume);
        }
        // && std::abs(pickupSample) < 0.00001f

        //hang leallitasi feltetelek
        if (!hammerModule.isHammerActive() && !toneBarModule.isToneBarActive() && !isKeyHeld) {
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
    tremolo.setTremRate(6.0f);
    tremolo.setDepth(0.7f);

    dcBlocker.prepare(specs);
    dcBlocker.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(specs.sampleRate, 10.0f);

    thumpFilter.prepare(specs);
    thumpFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(specs.sampleRate, 80.0f);
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