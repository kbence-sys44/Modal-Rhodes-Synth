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

    float noise = (random.nextFloat() * 2.0f - 1.0f) * 0.003f;
    float signalAbs = std::abs(inputSample);
    float breathingNoise = noise * (0.5f + (signalAbs * 10.0f));

    float noisyInput = inputSample + breathingNoise;

    float drive = 2.0f;
    float x = noisyInput * drive;

    float magneticSignal;
    if (x > 0) {
        magneticSignal = x; //poz - tiszta
    }
    else {
        magneticSignal = std::tanh(x) * 1.5f; //negativ - torz
    }

    magneticSignal = magneticSignal + (magneticSignal * magneticSignal * 0.2f);


   /* 3 verzio 
   float sample = noisyInput;
    float offset = 1.0f;
    float signal = sample + offset;
    float magneticSignal = sample + (0.35f * sample * sample);*/

    float processedSample = bassFilter.processSample(magneticSignal);
    processedSample = midFilter.processSample(processedSample);
    processedSample = trebleFilter.processSample(processedSample);

    /* 2 verzio
    float inputAbs = std::abs(sample);
    envelopeFollow = (envelopeFollow * 0.999f) + (inputAbs * 0.001f);

    //delay modulacio
    float modDepth = 5.0f;
    float modulatedDelay = baseDelay + (envelopeFollow * modDepth);
    modulatedDelay = juce::jlimit(2.0f, 80.0f, modulatedDelay);
    pickupDL.setDelay(modulatedDelay);

    pickupDL.pushSample(0, sample);
    float delayedSample = pickupDL.popSample(0);

    float pickupMix = 0.45f;
    float pickupSample = sample - (delayedSample * pickupMix);*/

    /*
    
    float bias = 0.3f;
    float biasSignal = pickupSample * bias;
    float saturatedSample = std::tanh(biasSignal * drive);

    //dc offset elkerulese
    float dcCorrection = std::tanh(bias * drive);
    saturatedSample -= dcCorrection;*/

    //float compensationSample = saturatedSample * (1.2f / (1.0f + (drive * 0.15f)));


    return physicalFilter.processSample(processedSample) * 0.4f;
}