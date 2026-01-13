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
//specs - sample rate es blokkmeretet tartalmaz
void DelayLineModule::prepareDelayLine(const juce::dsp::ProcessSpec& specs) {
    sampleRate = specs.sampleRate;

    auto maxDelayInSamples = (int)(2.0 * sampleRate); //max keslelteles - jelenleg 2mp

    delayLine.setMaximumDelayInSamples(maxDelayInSamples);
    delayLine.prepare(specs);
    delayLine.reset();

    lowpassFilter.prepare(specs);
    lowpassFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);

    //allpass lanc inicializalasa

    allpassChain.resize(4);
    for (auto& ap : allpassChain) {
        ap.prepare(specs);
        ap.setType(juce::dsp::FirstOrderTPTFilterType::allpass);
    }

    //szurok inicializalasa, ezek regiek, nem olyan stabil
   /* IIRfilter.prepare(specs);
    allpassFilter.prepare(specs);

    //egyszeru alulatereszto szuro beallitasa
    IIRfilter.coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 2500.0f);*/
}


//mindent visszaallit
void DelayLineModule::resetDelayLine() {
    delayLine.reset();
    lowpassFilter.reset();
    for (auto& ap : allpassChain) {
        ap.reset();
    }
    //IIRfilter.reset();
}

void DelayLineModule::setDelayForDelayLine(float delayInSamples, float velocity) {
    
    float frequency = float(sampleRate) / delayInSamples;
    float targetDelay = (float)sampleRate / frequency;

    //lowpass
    float brightness = juce::jmap(velocity, 0.0f, 1.0f, 15.0f, 45.0f);
    float cutoffOffset = (delayInSamples > 200.0f) ? 500.0f : 200.0f;
    float cutoff = cutoffOffset + (frequency * brightness);
    lowpassFilter.setCutoffFrequency(juce::jlimit(200.0f, 8000.0f, cutoff));

    //allpass
    float stiffness = frequency * 10.0f;
    stiffness = juce::jlimit(1000.0f, 20000.0f, stiffness);
    
    for (auto& ap : allpassChain) {
        ap.setCutoffFrequency(stiffness);
    }

    //fazis korrekcio a filterek altal okozott detuning javitasara

    double totalDelayPhase = 0.0;
    double singlePhase = getTPTPhase(frequency, stiffness);
    totalDelayPhase = singlePhase * (double)allpassChain.size();
   
    double omega = 2.0 * juce::MathConstants<double>::pi * frequency / sampleRate;

    float filterDS = 0.0f;
    if (std::abs(omega) > 1e-9) {
        filterDS = (float)(-totalDelayPhase / omega); // Delay = -phase/omega
    }
    
    float compensationDelay = delayInSamples - filterDS;
    compensationDelay = juce::jlimit(1.0f, (float)delayLine.getMaximumDelayInSamples() - 1.0f, compensationDelay);
    delayLine.setDelay(compensationDelay);

    /*
    //alap adatok, szurok bealliasa
    float noteFrequency = float(sampleRate) / delayInSamples;
    float brightness = juce::jmap(velocity, 0.0f, 1.0f, 15.0f, 45.0f);
    float cutoffOffset = (delayInSamples > 200.0f) ? 500.0f : 200.0f;
    float cutoffFrequency = cutoffOffset + (noteFrequency * brightness); // a melyebb hangokat hozzaadjuk hogy megmaradjanak es ne vagjuk le
    cutoffFrequency = juce::jlimit(400.0f, 7000.0f, cutoffFrequency);

    auto newLPCiacs = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, cutoffFrequency);
    IIRfilter.coefficients = newLPCiacs;

    //allpass szuro
    float stiffness;
    if (delayInSamples > 800.0f) {
        stiffness = juce::jmap(delayInSamples, 800.0f, 3000.0f, 40.0f, 100.0f);
    }
    else {
       stiffness = juce::jmap(delayInSamples, 50.0f, 800.0f, 2.0f, 25.0f);
    }

    float apFreq = stiffness * 100.0f;
    apFreq = juce::jlimit(200.0f, 16000.0f, apFreq);
    auto newAPCiacs = juce::dsp::IIR::Coefficients<float>::makeAllPass(sampleRate, apFreq);
    allpassFilter.coefficients = newAPCiacs;

    //phase delay comp
    double omega = 2.0 * juce::MathConstants<double>::pi * noteFrequency / sampleRate;

    double lpPhase = newLPCiacs->getPhaseForFrequency(noteFrequency, sampleRate);
    double apPhase = newAPCiacs->getPhaseForFrequency(noteFrequency, sampleRate);

    //fazis atfordulasnal
    if (lpPhase > 0.0) lpPhase -= 2.0 * juce::MathConstants<double>::pi;
    if (apPhase > 0.0) apPhase -= 2.0 * juce::MathConstants<double>::pi;

    //fazistolas = -fazis/delay
    //float lpDelayInSamples = (lpPhase == 0.0) ? 0.0f : (float)(-lpPhase / omega);
    //float apDelayInSamples = (apPhase == 0.0) ? 0.0f : (float)(-apPhase / omega);
    float lpDelayInSamples = (omega > 1e-9) ? (float)(-lpPhase / omega) : 0.0f;
    float apDelayInSamples = (omega > 1e-9) ? (float)(-apPhase / omega) : 0.0f;

    float totalFilterDelay = lpDelayInSamples + apDelayInSamples;
    totalFilterDelay = juce::jlimit(0.0f, 20.0f, totalFilterDelay);

    float targetDelay = delayInSamples - totalFilterDelay;
    currentDelay = juce::jlimit(1.0f, (float)delayLine.getMaximumDelayInSamples() - 1.0f, targetDelay);

    /*float tuningOffset = 0.5f;
    float targetDelay = delayInSamples - tuningOffset;
    currentDelay = juce::jlimit(1.0f, (float)delayLine.getMaximumDelayInSamples() - 1.0f, targetDelay);*/

    //delayLine.setDelay(currentDelay);

}

float DelayLineModule::processSample(float inputSample, float gain) {

    //random zaj
    //float jitterAmount = 0.1f;
    //float jitter = (random.nextFloat() * 2.0f - 1.0f) * jitterAmount;

    //delayLine.setDelay(currentDelay);

    float delayedSample = delayLine.popSample(0);
    float filteredSample = lowpassFilter.processSample(0, delayedSample); //lowpass

    for (auto& ap : allpassChain) { //allpass lanc
        filteredSample = ap.processSample(0, filteredSample);
    }

    float feedback = filteredSample * gain;
    float nextInput = inputSample + feedback;

    if (std::abs(nextInput) > 4.0f) nextInput *= 0.25f; //limit

    delayLine.pushSample(0, nextInput);

    return filteredSample;

   /* auto delayedSample = delayLine.popSample(0);
    auto filteredDelayedSample = IIRfilter.processSample(delayedSample);
    auto stiffSample = allpassFilter.processSample(filteredDelayedSample);

    auto feedbackSample = stiffSample * gain;
    

    auto outputSample = inputSample + feedbackSample;
    
    delayLine.pushSample(0, outputSample);

    return delayedSample;*/
}

double DelayLineModule::getTPTPhase(double frequency, double cutoff) {

    if (frequency <= 0.0 || cutoff < 0.0) return 0.0;

    double omegaFreq = 2.0 * juce::MathConstants<double>::pi * frequency / sampleRate;
    double omegaCut = 2.0 * juce::MathConstants<double>::pi * cutoff / sampleRate;

    double g = std::tan(omegaCut * 0.5); //warp faktor
    std::complex<double> z = std::polar(1.0, -omegaFreq); // z = e^(j*omega), ez inverz

    std::complex<double> num = (g - 1.0) + (g + 1.0) * z;
    std::complex<double> den = (g + 1.0) + (g + 1.0) * z;
    std::complex<double> H = num / den;

    return std::arg(H);
}