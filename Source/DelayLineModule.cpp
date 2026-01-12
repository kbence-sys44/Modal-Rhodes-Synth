/*
  ==============================================================================

    DelayLine.cpp
    Created: 19 Dec 2025 5:48:52pm
    Author:  kadar

  ==============================================================================
*/

#include "DelayLineModule.h"

//Ez az osztaly felel a ketiranyu hullam megvalositasaert


//DelayLine modul elokeszitese
//specifications - sample rate es blokkmeretet tartalmaz
void DelayLineModule::prepareDelayLine(const juce::dsp::ProcessSpec& specifications) {
    sampleRate = specifications.sampleRate;

    auto maxDelayInSamples = (int)(2.0 * sampleRate); //max keslelteles - jelenleg 2mp


    delayLine.setMaximumDelayInSamples(maxDelayInSamples);
    delayLine.prepare(specifications);

    delayLine.reset();



    //szurok inicializalasa
    IIRfilter.prepare(specifications);
    allpassFilter.prepare(specifications);

    //egyszeru alulatereszto szuro beallitasa
    IIRfilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 2500.0f);

    

}


//mindent visszaallit
void DelayLineModule::resetDelayLine() {
    delayLine.reset();
    IIRfilter.reset();
}

void DelayLineModule::setDelayForDelayLine(float delayInSamples, float velocity) {
    //alap adatok, szurok bealliasa
    float noteFrequency = float(sampleRate) / delayInSamples;
    float brightness = juce::jmap(velocity, 0.0f, 1.0f, 15.0f, 45.0f);
    float cutoffOffset = (delayInSamples > 200.0f) ? 500.0f : 200.0f;
    float cutoffFrequency = cutoffOffset + (noteFrequency * brightness); // a melyebb hangokat hozzaadjuk hogy megmaradjanak es ne vagjuk le
    cutoffFrequency = juce::jlimit(400.0f, 16000.0f, cutoffFrequency);

    auto newLPCiacs = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, cutoffFrequency);
    IIRfilter.coefficients = newLPCiacs;

    //allpass szuro
    float stiffness;
    if (delayInSamples > 800.0f) {
        stiffness = juce::jmap(delayInSamples, 800.0f, 3000.0f, 40.0f, 150.0f);
    }
    else {
       stiffness = juce::jmap(delayInSamples, 50.0f, 80000.0f, 5.0f, 40.0f);
    }

    float apFreq = stiffness * 100.0f;
    apFreq = juce::jlimit(200.0f, 18000.0f, apFreq);
    auto newAPCiacs = juce::dsp::IIR::Coefficients<float>::makeAllPass(sampleRate, apFreq);
    allpassFilter.coefficients = newAPCiacs;

    //phase delay comp
    double omega = 2.0 * juce::MathConstants<double>::pi * noteFrequency / sampleRate;

    double lpPhase = newLPCiacs->getPhaseForFrequency(noteFrequency, sampleRate);
    double apPhase = newAPCiacs->getPhaseForFrequency(noteFrequency, sampleRate);

    //fazistolas = -fazis/delay
    float lpDelayInSamples = (lpPhase == 0.0) ? 0.0f : (float)(-lpPhase / omega);
    float apDelayInSamples = (apPhase == 0.0) ? 0.0f : (float)(-apPhase / omega);

    float totalFilterDelay = lpDelayInSamples + apDelayInSamples;
    float targetDelay = delayInSamples - totalFilterDelay;
    currentDelay = juce::jlimit(1.0f, (float)delayLine.getMaximumDelayInSamples() - 1.0f, targetDelay);
    delayLine.setDelay(currentDelay);

}

float DelayLineModule::processSample(float inputSample, float gain) {

    //random zaj
    float jitterAmount = 0.1f;
    float jitter = (random.nextFloat() * 2.0f - 1.0f) * jitterAmount;

    delayLine.setDelay(currentDelay + jitter);


    auto delayedSample = delayLine.popSample(0);
    auto filteredDelayedSample = IIRfilter.processSample(delayedSample);
    auto stiffSample = allpassFilter.processSample(filteredDelayedSample);

    auto feedbackSample = stiffSample * gain;
    

    auto outputSample = inputSample + feedbackSample;
    
    delayLine.pushSample(0, outputSample);

    return delayedSample;
}