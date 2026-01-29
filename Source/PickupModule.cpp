/*
  ==============================================================================

    PickupModule.cpp
    Created: 2 Jan 2026 7:19:57pm
    Author:  kadar

  ==============================================================================
*/

#include "PickupModule.h"


void PickupModule::prepare(const juce::dsp::ProcessSpec& specs) {

    sampleRate = specs.sampleRate;

    lowpass.prepare(specs);
    lowpass.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    lowpass.setResonance(0.707f);

    highpass.prepare(specs);
    highpass.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    highpass.setResonance(0.707f);

    gain.reset(sampleRate, 0.02);
    symmetryGain.reset(sampleRate, 0.02);

    setParameters(0.0f, 0.0f, 6000.0f);
}

void PickupModule::reset() {

    lowpass.reset();
    highpass.reset();

    gain.reset(sampleRate, 0.02);
    symmetryGain.reset(sampleRate, 0.02);

}

void PickupModule::setParameters(float gainDB, float symmetryDB, float lowPassCutoff) {

    gain.setTargetValue(juce::Decibels::decibelsToGain(gainDB));
    symmetryGain.setTargetValue(juce::Decibels::decibelsToGain(symmetryDB));
    lowpass.setCutoffFrequency(lowPassCutoff);

}

void PickupModule::setFrequency(float frequency) {
    if (frequency > 20.0f) {
        highpass.setCutoffFrequency(frequency);
    }

}

//a hangszedo fo feldolgozo fuggvenye
float PickupModule::processSample(float inputSample) {

    float processedSample = lowpass.processSample(0, inputSample);

    float currentGain = gain.getNextValue();
    processedSample *= currentGain;//gain
    processedSample = std::tanh(processedSample);//soft clipping

    float symmetry = symmetryGain.getNextValue();

    if (symmetry > 1.01f) { //asszimetria

        float path1 = processedSample * symmetry;
        
        path1 = std::pow(2.0f, path1);
        path1 -= 1.0f;

        float path2 = 1.0f / std::pow(2.0f, symmetry);
        processedSample = path1 * path2;
    }

    
    processedSample = processedSample - (std::pow(processedSample, 3.0f) / 3.0f);//buzz

    processedSample = highpass.processSample(0, processedSample);
    return processedSample;

    /*

    float clean = inputSample * drive;

    //asszimetria
    float bias = 0.3f;
    float biasedSig = clean + bias;

    //magneses sat
    float satSig;
    if (biasedSig > 0) {
        satSig = biasedSig / (1.0f + biasedSig); //lapos
    }
    else {
        satSig = biasedSig / (1.0f + std::abs(biasedSig) * 0.5f); //negativ -hegyes
    }

    satSig -= (bias / 1.0f + bias); //dc offset eltavolitasa, high pass

    float eqdSignal = bassFilter.processSample(satSig);
    eqdSignal = midFilter.processSample(eqdSignal);

    float smoothSig = physicalFilter.processSample(eqdSignal);//kabel szimulacio

    float finalSig = trebleFilter.processSample(smoothSig);

    return finalSig; 
 */
}