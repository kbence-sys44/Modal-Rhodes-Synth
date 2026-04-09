/*
  ==============================================================================

    PickupModule.cpp
    Created: 2 Jan 2026 7:19:57pm
    Author:  kadar

  ==============================================================================
*/

//ez a modul a kapott tine elmozdulast elektromos jelle alakitja

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

    setParameters(0.0f, 0.0f, 12000.0f);
}

void PickupModule::reset() {

    lowpass.reset();
    highpass.reset();

    gain.reset(sampleRate, 0.02);
    symmetryGain.reset(sampleRate, 0.02);

}

void PickupModule::setParameters(float gainDB, float symmetryVal, float lowPassCutoff) {

    gain.setTargetValue(juce::Decibels::decibelsToGain(gainDB));
    symmetryGain.setTargetValue(juce::Decibels::decibelsToGain(symmetryVal));
    lowpass.setCutoffFrequency(lowPassCutoff);

}

void PickupModule::setFrequency(float frequency) {
    if (frequency > 20.0f) {
        highpass.setCutoffFrequency(frequency);
    }

}

//a hangszedo fo feldolgozo fuggvenye
//a bemeneti jelet koszolja es alakitja at
float PickupModule::processSample(float inputSample) {

    float processedSample = lowpass.processSample(0, inputSample);

    float currentGain = gain.getNextValue();
    processedSample *= currentGain;//gain
    processedSample = std::tanh(processedSample);//analog torzitas

    float symmetry = symmetryGain.getNextValue(); //paros felharmonikusok generalasa, mivel a magnes nincs tokeletesen kozepen

    if (symmetry > 0.01f) { //asszimetria

        float path1 = processedSample * symmetry;
        
        path1 = std::pow(2.0f, path1);
        path1 -= 1.0f;

        float path2 = 1.0f / std::pow(2.0f, symmetry);
        processedSample = path1 * path2;
    }

    
    //processedSample = processedSample - (std::pow(processedSample, 3.0f) / 3.0f);//buzz

    processedSample = highpass.processSample(0, processedSample); //melyek tisztitasa
    return processedSample;
}