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
    midPeakFilter.prepare(specs);
    dcBlocker.prepare(specs);

    dcBlocker.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 10.0f); //fix 10hz highpass

    reset();
    updateFilters();
}

void Preamp::reset() {
    bassFilter.reset();
    trebleFilter.reset();
    midPeakFilter.reset();
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
    midPeakFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 400.0f, 0.8f, 1.4f);
}

float Preamp::processSample(float inputSample) {

    float processedSignal = inputSample * drive; //drive
    processedSignal = std::tanh(processedSignal); //sat
    processedSignal = dcBlocker.processSample(processedSignal); //dcblock

    processedSignal = bassFilter.processSample(processedSignal); //eq
    processedSignal = midPeakFilter.processSample(processedSignal);
    processedSignal = trebleFilter.processSample(processedSignal);
    
    return processedSignal * outputVolume;

    //2, 3 felharmonikus erositese, egyelore nem kell
    /*float drySignal = inputSample * drive;

    float saturated = std::tanh(drySignal); //-1 es 1 koze tesszuk

    float harmonic2 = saturated * saturated;
    float harmonic3 = saturated * saturated * saturated;

    float h2Gain = 0.5f * (drive * 0.5f);
    float h3Gain = 0.3f * (drive * 0.3f);

    float mixedSignal = saturated + (harmonic2 * h2Gain) + (harmonic3 * h3Gain);
    float dcFiltered = mixedSignal - prevInput + (0.997f * prevOutput); //high pass, dc blocker
    
    prevInput = mixedSignal;
    prevOutput = dcFiltered;

    float output = dcFiltered * (0.5f / (1.0f + (drive * 0.2f)));*/  
}