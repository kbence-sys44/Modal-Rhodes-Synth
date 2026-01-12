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
    bassFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 90.0f, 0.7f, 1.0f);

    midFilter.prepare(specs);
    midFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 500.0f, 0.6f, 0.85f);

    trebleFilter.prepare(specs);
    trebleFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 2200.0f, 1.0f, 1.3f);

    physicalFilter.prepare(specs);
    physicalFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 3200.0f, 1.1f);

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

    float sample = inputSample;
    sample = bassFilter.processSample(sample);
    sample = midFilter.processSample(sample);
    sample = trebleFilter.processSample(sample);

    float inputAbs = std::abs(sample);
    envelopeFollow = (envelopeFollow * 0.999f) + (inputAbs * 0.001f);

    //delay modulacio
    float modulatedDelay = baseDelay + (envelopeFollow * 40.0f);
    modulatedDelay = juce::jlimit(1.0f, 99.0f, modulatedDelay);
    pickupDL.setDelay(modulatedDelay);

    pickupDL.pushSample(0, sample);
    float delayedSample = pickupDL.popSample(0);

    float pickupSample = sample - (delayedSample * 0.3f);

    /*
    
    float bias = 0.3f;
    float biasSignal = pickupSample * bias;
    float saturatedSample = std::tanh(biasSignal * drive);

    //dc offset elkerulese
    float dcCorrection = std::tanh(bias * drive);
    saturatedSample -= dcCorrection;*/

    //float compensationSample = saturatedSample * (1.2f / (1.0f + (drive * 0.15f)));


    return physicalFilter.processSample(pickupSample);
}