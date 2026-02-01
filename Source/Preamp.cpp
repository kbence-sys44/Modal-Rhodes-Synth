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

    outputGain.setGainDecibels(0.0f);
    inputGain.setGainDecibels(0.0f);

    bassFilter.state = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 80.0f, 0.707f, 1.0f);
    trebleFilter.state = juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 4000.0f, 0.707f, 1.0f);
    dcBlocker.state = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 20.0f);

    bassFilter.prepare(specs);
    trebleFilter.prepare(specs);
    dcBlocker.prepare(specs);

    reset();
}

void Preamp::reset() {
    inputGain.reset();
    outputGain.reset();
    bassFilter.reset();
    trebleFilter.reset();
    dcBlocker.reset();
}

void Preamp::setDrive(float newDrive) {
    inputGain.setGainDecibels(newDrive);
}

void Preamp::setOutputLevel(float newDB) {
    outputGain.setGainDecibels(newDB);
}

void Preamp::setBassGain(float newBassGain) {
    float bassGainLin = juce::Decibels::decibelsToGain(newBassGain);
    *bassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate,80.0f, 0.707f, bassGainLin);
}

void Preamp::setTrebleGain(float newTrebleGain) {
    float trebleGainLin = juce::Decibels::decibelsToGain(newTrebleGain);
    *trebleFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 4000.0f, 0.707f, trebleGainLin);
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

    //filter lanc
    dcBlocker.process(context); 
    bassFilter.process(context);
    trebleFilter.process(context);

    //kimeneti erosites
    outputGain.process(context);

}

float Preamp::applySat(float inputSample) {

    float biasedSample = inputSample + saturationBias;
    
    float saturatedSample = std::tanh(biasedSample);

    return (saturatedSample - std::tanh(saturationBias)) * 1.2f;
}