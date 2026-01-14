/*
  ==============================================================================

    Preamp.cpp
    Created: 11 Jan 2026 8:21:45pm
    Author:  kadar

  ==============================================================================
*/

#include "Preamp.h"

void::Preamp::prepare(const::juce::dsp::ProcessSpec& specs) {
    sampleRate = specs.sampleRate;

    bassFilter.prepare(specs);
    trebleFilter.prepare(specs);

    dcBlocker.prepare(specs);
    dcBlocker.setType(juce::dsp::StateVariableTPTFilterType::highpass); //fix 10hz highpass
    dcBlocker.setCutoffFrequency(10.0f);

    reset();
    updateFilters();
}

void Preamp::reset() {
    bassFilter.reset();
    trebleFilter.reset();
    dcBlocker.reset();
}

void Preamp::setDrive(float newDrive) {
    drive = juce::jlimit(0.0f, 20.0f, newDrive);
}

void Preamp::setBassGain(float newBassGain) {
    bassGain = juce::Decibels::decibelsToGain(newBassGain);
    updateFilters();
}

void Preamp::setTrebleGain(float newTrebleGain) {
    trebleGain = juce::Decibels::decibelsToGain(newTrebleGain);
    updateFilters();
}

void Preamp::setVolume(float newVolume) {
    outputVolume = juce::jlimit(0.0f, 2.0f, newVolume);
}

void Preamp::updateFilters() {
    if (sampleRate <= 0.0) return;

    bassFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 90.0f, 0.707f, bassGain);
    trebleFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 2500.0f, 0.707f, trebleGain);
}

float Preamp::processSample(float inputSample) {

    float processedSignal = bassFilter.processSample(inputSample); //eq
    processedSignal = trebleFilter.processSample(processedSignal);

    processedSignal = inputSample * drive;//drive

    processedSignal = std::tanh(processedSignal); //sat

    processedSignal = dcBlocker.processSample(1, processedSignal); //dcblock

    return processedSignal * outputVolume; 
}