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
    float brightness = juce::jmap(velocity, 0.0f, 1.0f, 1.5f, 6.0f);
    float cutoffOffset = (delayInSamples > 200.0f) ? 800.0f : 200.0f;
    float cutoffFrequency = cutoffOffset + (noteFrequency * brightness); // a melyebb hangokat hozzaadjuk hogy megmaradjanak es ne vagjuk le
    cutoffFrequency = juce::jlimit(200.0f, 18000.0f, cutoffFrequency);

    auto newLPCiacs = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, cutoffFrequency);
    IIRfilter.coefficients = newLPCiacs;

    //allpass szuro
    float stiffness = juce::jmap(delayInSamples, 50.0f, 1500.0f, 10.0f, 40.0f);
    stiffness = juce::jlimit(5.0f, 100.0f, stiffness);

    auto newAPCiacs = juce::dsp::IIR::Coefficients<float>::makeAllPass(sampleRate, stiffness);
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

    //a linearis interpolaciot a delayLine osztaly automatikusan kezeli
    auto delayedSample = delayLine.popSample(0);

    //a kimenet egy reszet visszavezetjuk a bemenetre es szurjuk a csillapitas miatt
    auto filteredDelayedSample = IIRfilter.processSample(delayedSample);
    //+ allpass
    auto stiffSample = allpassFilter.processSample(filteredDelayedSample);

    auto feedbackSample = stiffSample * gain;
    

    auto outputSample = inputSample + feedbackSample;
    
    delayLine.pushSample(0, outputSample);

    return delayedSample;
}