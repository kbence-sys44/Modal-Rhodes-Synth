/*
  ==============================================================================

    PickupModule.cpp
    Created: 2 Jan 2026 7:19:57pm
    Author:  kadar

  ==============================================================================
*/

#include "PickupModule.h"


void PickupModule::preparePickup(double sampleRate) {
    this->sampleRate = sampleRate;
    reset();

    toneFilter.prepare({sampleRate, 512, 1});
    toneFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 6000.0f);
}

void PickupModule::reset() {
    lastInputSample = 0.0f;
    toneFilter.reset();

}

void PickupModule::setDrive(float newDrive) {

    drive = juce::jlimit(1.0f, 10.0f, newDrive);

}

float PickupModule::processSignal(float inputSample) {

    //pickup poz
    float filteredSample = inputSample - (lastInputSample * 0.3f);
    lastInputSample = inputSample;

    //tone
    filteredSample = toneFilter.processSample(filteredSample);

    //szaturci
    float saturatedSample = std::tanh(filteredSample * drive);

    //kompenzls
    float output = saturatedSample * (1 / sqrt(drive));

    return output;
}