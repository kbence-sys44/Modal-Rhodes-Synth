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
    reset();

    bassFilter.prepare(specs);
    bassFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 120.0f, 1.0f, 2.0f);

    midFilter.prepare(specs);
    midFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 500.0f, 0.8f, 0.7f);

    trebleFilter.prepare(specs);
    trebleFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 3000.0f, 1.0f, 1.4f);

    //toneFilter.prepare(specs);
    //toneFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 3000.0f, 1.5f, 2.0f);

    pickupDL.setMaximumDelayInSamples(100);
    pickupDL.prepare(specs);
    pickupDL.setDelay(14.0f);
}

void PickupModule::reset() {
    lastInputSample = 0.0f;
    //toneFilter.reset();

    bassFilter.reset();
    midFilter.reset();
    trebleFilter.reset();
}

void PickupModule::setDrive(float newDrive) {

    drive = juce::jlimit(1.0f, 10.0f, newDrive);

}

//a hangszedo fo feldolgozo fuggvenye
float PickupModule::processSignal(float inputSample) {

    float sample = inputSample;
    sample = bassFilter.processSample(sample);
    sample = midFilter.processSample(sample);
    sample = trebleFilter.processSample(sample);

    pickupDL.pushSample(0, sample);
    float delayedSample = pickupDL.popSample(0);

    float pickupSample = sample - (delayedSample * 0.25f);

    float saturatedSample = std::tanh(pickupSample * drive + (pickupSample * pickupSample * 0.2f));

    float output = saturatedSample * (1 / sqrt(drive));

    return output;
}