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


    //szűrő inicializálása
    IIRfilter.prepare(specifications);

    //egyszerű aluláteresztő szűrő beállítása
    IIRfilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 20000.0f);

}


//mindent visszaállít
void DelayLineModule::resetDelayLine() {
    delayLine.reset();
    IIRfilter.reset();
}

void DelayLineModule::setDelayForDelayLine(float delayInSamples) {
    //esetünkben a delay = sampleRate / frekvencia

    currentDelay = juce::jlimit(1.0f, (float)delayLine.getMaximumDelayInSamples() - 1.0f, delayInSamples);

    delayLine.setDelay(currentDelay);
}

float DelayLineModule::processSample(float inputSample, float gain) {

    //a lineáris interpolációt a delayLine osztály automatikusan kezeli
    auto delayedSample = delayLine.popSample(0);

    //a kimenet egy részét visszavezetjük a bemenetre és szűrjük a csillapítás miatt
    
    auto filteredDelayedSample = IIRfilter.processSample(delayedSample);
    auto feedbackSample = filteredDelayedSample * gain;
    

    auto outputSample = inputSample + feedbackSample;
    
    delayLine.pushSample(0, outputSample);

    return delayedSample;
}