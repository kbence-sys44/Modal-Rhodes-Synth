/*
  ==============================================================================

    Preamp.cpp
    Created: 11 Jan 2026 8:21:45pm
    Author:  kadar

  ==============================================================================
*/

//a hangszin szabalyozasert felelos modul
//lenyegeben egy filter lanc

#include "Preamp.h"

void::Preamp::prepare(const::juce::dsp::ProcessSpec& specs) {
    sampleRate = specs.sampleRate;

    inputGain.prepare(specs);
    inputGain.setRampDurationSeconds(0.05); //tekeres atmenet
    outputGain.prepare(specs);
    outputGain.setRampDurationSeconds(0.05);

    inputGain.setGainDecibels(0.0f);
    outputGain.setGainDecibels(0.0f);

    for (int i = 0; i < 2; ++i) {
        bassFilter[i].prepare(specs);
        trebleFilter[i].prepare(specs);

        auto bassCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 200.0f, 0.707f, 1.0f);
        auto trebleCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 2500.0f, 0.707f, 1.0f);

        bassFilter[i].coefficients = bassCoeffs;
        trebleFilter[i].coefficients = trebleCoeffs;

        dcBlocker[i].prepare(specs);
        dcBlocker[i].setType(juce::dsp::StateVariableTPTFilterType::highpass);
        dcBlocker[i].setCutoffFrequency(30.0f);
    }

    reset();
}

void Preamp::reset() {
    inputGain.reset();
    outputGain.reset();
    
    for (int i = 0; i < 2; ++i) {
        bassFilter[i].reset();
        trebleFilter[i].reset();
        dcBlocker[i].reset();
    }
}

void Preamp::setDrive(float newDrive) {
    inputGain.setGainDecibels(newDrive);
}

void Preamp::setOutputLevel(float newDB) {
    outputGain.setGainDecibels(newDB);
}

void Preamp::setBassGain(float newBassGain) {
    //if (std::abs(newBassGain - lastBassGain) < 0.01f) return;
 
    float bassGainLin = juce::Decibels::decibelsToGain(newBassGain);
    auto newCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 80.0f, 0.707f, bassGainLin);

    bassFilter[0].coefficients = newCoeffs;
    bassFilter[1].coefficients = newCoeffs;
}

void Preamp::setTrebleGain(float newTrebleGain) {
    //if (std::abs(newTrebleGain - lastTrebleGain) < 0.01f) return;

    float trebleGainLin = juce::Decibels::decibelsToGain(newTrebleGain);
    auto newCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 4000.0f, 0.707f, trebleGainLin);

    trebleFilter[0].coefficients = newCoeffs;
    trebleFilter[1].coefficients = newCoeffs;
}



void Preamp::process(juce::dsp::ProcessContextReplacing<float>& context) {

    auto& block = context.getOutputBlock();

    inputGain.process(context); //bemeneti jel erositese

    for (size_t channel = 0; channel < block.getNumChannels(); ++channel) {
        auto* blockData = block.getChannelPointer(channel);

        for (size_t i = 0; i < block.getNumSamples(); ++i) {
            blockData[i] = applySat(blockData[i]); //szaturacios reteg
        }
    }

    //filter lanc olalankent
    if (block.getNumChannels() > 0) {
        auto leftBlock = block.getSingleChannelBlock(0);
        juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);

        dcBlocker[0].process(leftContext);
        bassFilter[0].process(leftContext);
        trebleFilter[0].process(leftContext);
    }

    if (block.getNumChannels() > 1) {
        auto rightBlock = block.getSingleChannelBlock(1);
        juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

        dcBlocker[1].process(rightContext);
        bassFilter[1].process(rightContext);
        trebleFilter[1].process(rightContext);
    }

    //kimeneti erosites
    outputGain.process(context);

}

float Preamp::applySat(float inputSample) {

    float biasedSample = inputSample + saturationBias;
    
    float saturatedSample = std::tanh(biasedSample);

    return (saturatedSample - std::tanh(saturationBias)) * 1.2f;
}