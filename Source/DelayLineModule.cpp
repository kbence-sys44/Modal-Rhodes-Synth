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

void DelayLineModule::setDelayForDelayLine(float delayInSamples) {
    //esetunkben a delay = sampleRate / frekvencia

    //1.0 es 2.0 kozott kiserletezni kell vele
    float filterDelayCompensation = 1.2f;

    float targetDelay = delayInSamples - filterDelayCompensation;

    currentDelay = juce::jlimit(1.0f, (float)delayLine.getMaximumDelayInSamples() - 1.0f, targetDelay);

    delayLine.setDelay(currentDelay);


    float noteFrequency = (float)sampleRate / delayInSamples;

    //hangmagassag alapjan meghatarozott filter utesenkent
    float cutoffFrequency = noteFrequency * 8.0f;

    //lekorlatozzuk
    cutoffFrequency = juce::jlimit(200.0f, 18000.0f, cutoffFrequency);

    IIRfilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, cutoffFrequency);

    //allpass szuro
    float stiffness = (delayInSamples > 150.0f) ? 60.0f : 15.0f;

    allpassFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeAllPass(sampleRate, stiffness);
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