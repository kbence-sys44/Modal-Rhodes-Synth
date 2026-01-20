/*
  ==============================================================================

    PickupModule.cpp
    Created: 2 Jan 2026 7:19:57pm
    Author:  kadar

  ==============================================================================
*/

#include "PickupModule.h"


void PickupModule::preparePickup(const juce::dsp::ProcessSpec& specs) {
    sampleRate = specs.sampleRate;
    
    bassFilter.prepare(specs);
    bassFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 200.0f, 0.7f, 2.0f);

    midFilter.prepare(specs);
    midFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 380.0f, 0.5f, 1.5f);

    trebleFilter.prepare(specs);
    trebleFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 3000.0f, 0.7f, 0.9f);

    physicalFilter.prepare(specs);
    physicalFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 2500.0f, 1.5f);

    pickupDL.setMaximumDelayInSamples(100);
    pickupDL.prepare(specs);

    reset();
}

void PickupModule::reset() {
    lastInputSample = 0.0f;
    envelopeFollow = 0.0f;

    bassFilter.reset();
    midFilter.reset();
    trebleFilter.reset();
    physicalFilter.reset();

    pickupDL.setDelay(baseDelay);
}

void PickupModule::setDrive(float newDrive) {

    drive = juce::jlimit(1.0f, 10.0f, newDrive);

}

void PickupModule::setBassGain(float newGain) {
    bassFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 90.0f, 0.6f, newGain);
}

void PickupModule::setBaseDelay(float newDelay) {
    baseDelay = juce::jlimit(1.0f, 90.0f, newDelay);
}

//a hangszedo fo feldolgozo fuggvenye
float PickupModule::processSignal(float inputSample) {
    /*float offset = 0.45f;
    float position = inputSample + offset;

    float cleanSig = position;
    float drivenSig = cleanSig * (drive * 3.0f);

    float processedSig = std::tanh(drivenSig) + (0.25f * drivenSig * drivenSig);

    float input = inputSample * drive;
    float gap = 0.6f;
    float processedSig = input / (gap + std::abs(input));

    if (processedSig > 0.0f) {
        processedSig = std::tanh(processedSig * 1.2f);
    }
    else {
        processedSig = std::tanh(processedSig);
    }

    float drivenSig = inputSample * drive;
    float satSig = 0.0f;
    if (drivenSig > 1.0f) {
        satSig = 1.0f;
    }else if (drivenSig > 0.0f) {
        //satSig = std::tanh(drivenSig) / (1.0f + 0.2f * drivenSig * drivenSig);
        satSig = input - (input * input * input) / 3.0f;
    }
    else {
        satSig = std::tanh(drivenSig);
    }*/

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
 
}