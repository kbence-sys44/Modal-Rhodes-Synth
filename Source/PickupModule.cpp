/*
  ==============================================================================

    PickupModule.cpp
    Created: 2 Jan 2026 7:19:57pm
    Author:  kadar

  ==============================================================================
*/

#include "PickupModule.h"


void PickupModule::preparePickup(double sr) {
    sampleRate = sr;
    reset();

    toneFilter.prepare({ sampleRate, 512, 2 });

    toneFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 3000.0f, 1.5f, 2.0f);
}

void PickupModule::reset() {
    lastInputSample = 0.0f;
    toneFilter.reset();

}

void PickupModule::setDrive(float newDrive) {

    drive = juce::jlimit(1.0f, 10.0f, newDrive);

}

//a hangszedo fo feldolgozo fuggvenye
float PickupModule::processSignal(float inputSample) {

    float filteredSample = toneFilter.processSample(inputSample);

    float pickupSample = filteredSample - (lastInputSample * 0.5f);
    lastInputSample = filteredSample;

    float saturatedSample = std::tanh(pickupSample * drive + (pickupSample * pickupSample * 0.2f));

    float output = saturatedSample * (1 / sqrt(drive));

    return output;
}