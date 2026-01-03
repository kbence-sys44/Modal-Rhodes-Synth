/*
  ==============================================================================

    DelayLine.cpp
    Created: 19 Dec 2025 5:48:52pm
    Author:  kadar

  ==============================================================================
*/

#include "DelayLineModule.h"

//Ez az osztály felel a kétirányú hullám megvalósításáért


//DelayLine modul elõkészítése
//specifications - sample rate és blokkméretet tartalmaz
void DelayLineModule::prepareDelayLine(const juce::dsp::ProcessSpec& specifications) {
    sampleRate = specifications.sampleRate;

    auto maxDelayInSamples = (int)(2.0 * sampleRate); //max késleltelés - jelenleg 2mp


    delayLine.setMaximumDelayInSamples(maxDelayInSamples);
    delayLine.prepare(specifications);

    delayLine.reset();



    //szűrők inicializálása
    IIRfilter.prepare(specifications);
    allpassFilter.prepare(specifications);

    //egyszerű aluláteresztő szűrő beállítása
    IIRfilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 2500.0f);

    

}


//mindent visszaállít
void DelayLineModule::resetDelayLine() {
    delayLine.reset();
    IIRfilter.reset();
}

void DelayLineModule::setDelayForDelayLine(float delayInSamples) {
    //esetünkben a delay = sampleRate / frekvencia

    //1.0 és 2.0 között kísérletezni kell vele
    float filterDelayCompensation = 1.2f;

    float targetDelay = delayInSamples - filterDelayCompensation;

    currentDelay = juce::jlimit(1.0f, (float)delayLine.getMaximumDelayInSamples() - 1.0f, targetDelay);

    delayLine.setDelay(currentDelay);


    float noteFrequency = (float)sampleRate / delayInSamples;

    //hangmagasság alapján meghatározott filter ütésenként
    float cutoffFrequency = noteFrequency * 8.0f;

    //lekorlátozzuk
    cutoffFrequency = juce::jlimit(200.0f, 18000.0f, cutoffFrequency);

    IIRfilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, cutoffFrequency);

    //allpass szűrő
    float stiffness = (delayInSamples > 150.0f) ? 60.0f : 15.0f;

    allpassFilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeAllPass(sampleRate, stiffness);
}

float DelayLineModule::processSample(float inputSample, float gain) {

    //a lineáris interpolációt a delayLine osztály automatikusan kezeli
    auto delayedSample = delayLine.popSample(0);

    //a kimenet egy részét visszavezetjük a bemenetre és szűrjük a csillapítás miatt
    
    auto filteredDelayedSample = IIRfilter.processSample(delayedSample);
    //+ allpass
    auto stiffSample = allpassFilter.processSample(filteredDelayedSample);

    auto feedbackSample = stiffSample * gain;
    

    auto outputSample = inputSample + feedbackSample;
    
    delayLine.pushSample(0, outputSample);

    return delayedSample;
}