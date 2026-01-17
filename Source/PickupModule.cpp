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
    bassFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 80.0f, 0.7f, 1.3f);

    midFilter.prepare(specs);
    midFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 380.0f, 0.5f, 1.5f);

    trebleFilter.prepare(specs);
    trebleFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 3000.0f, 0.7f, 0.9f);

    physicalFilter.prepare(specs);
    physicalFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 2450.0f, 1.1f);

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

    float circuitNoise = (random.nextFloat() * 2.0f - 1.0f) * 0.002f;

    float inputAbs = std::abs(inputSample);
    envelopeFollow = (envelopeFollow * 0.99f) + (inputAbs * 0.01f);
    
    float proximity = 1.0f + (envelopeFollow * 4.0f);
    float rawSig = inputSample + circuitNoise;
    float drivenSig = rawSig * drive * proximity;

    float x = drivenSig;
    float sat = std::tanh(x + 0.2f) - 0.2f;

    //float polynom = x + (0.45f * x * x) - (0.15f * x * x * x); //asszimetria + szat
    //float magneticSignal = std::tanh(polynom);

    return physicalFilter.processSample(sat) * 0.35f; //kabel szimulacio
 
}