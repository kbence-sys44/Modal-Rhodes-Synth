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
}


//mindent visszaallit
void DelayLineModule::resetDelayLine() {
    delayLine.reset();
    lowpassFilter.reset();
    for (auto& ap : allpassChain) {
        ap.reset();
    }
}

void DelayLineModule::setDelayForDelayLine(float delayInSamples, float velocity, float frequency) {
   
    float targetDelay = (float)sampleRate / frequency;

    //lowpass
    float brightness = juce::jmap(velocity, 0.0f, 1.0f, 10.0f, 40.0f);
    float cutoffOffset = (delayInSamples > 200.0f) ? 500.0f : 200.0f;
    float cutoff = cutoffOffset + (frequency * brightness);

    float lowPassRes = 0.707f; //alapertelmezett
    lowpassFilter.setCutoffFrequency(juce::jlimit(400.0f, 5000.0f, cutoff));
    lowpassFilter.setResonance(lowPassRes);

    //allpass
    float stiffness = frequency * 60.0f;
    stiffness = juce::jlimit(1000.0f, 20000.0f, stiffness);
    
    for (auto& ap : allpassChain) {
        ap.setCutoffFrequency(stiffness);
    }

    //fazis korrekcio a filterek altal okozott detuning javitasara

    double totalDelayPhase = 0.0;
    totalDelayPhase += getTPTPhaseLow(frequency, cutoff, lowPassRes);
    double singlePhase = getTPTPhaseAllpass(frequency, stiffness);
    totalDelayPhase = singlePhase * (double)allpassChain.size();
   
    double omega = 2.0 * juce::MathConstants<double>::pi * frequency / sampleRate;

    float filterDS = 0.0f;
    if (std::abs(omega) > 1e-9) {
        filterDS = (float)(-totalDelayPhase / omega); // Delay = -phase/omega
    }

    float lagrangeLateny = 2.5f;
    
    float compensationDelay = delayInSamples - filterDS - lagrangeLateny;
    compensationDelay = juce::jlimit(1.0f, (float)delayLine.getMaximumDelayInSamples() - 1.0f, compensationDelay);
    delayLine.setDelay(compensationDelay);

}

float DelayLineModule::processSample(float inputSample, float gain) {
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
}

double DelayLineModule::getTPTPhaseAllpass(double frequency, double cutoff) {

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

double DelayLineModule::getTPTPhaseLow(double frequency, double cutoff, double q) {

    if (frequency <= 0.0 || cutoff < 0.0) return 0.0;

    double omegaFreq = 2.0 * juce::MathConstants<double>::pi * frequency / sampleRate;
    double omegaCut = 2.0 * juce::MathConstants<double>::pi * cutoff / sampleRate;

    double g = std::tan(omegaCut * 0.5); //warp faktor

    std::complex<double> z_inv = std::polar(1.0, -omegaFreq);
    std::complex<double> z = std::polar(1.0, omegaFreq);

    std::complex<double> one(1.0, 0.0);
    std::complex<double> s_map = (one / g) * ((z - one) / (z + one));

    std::complex<double> den = one + (s_map / q) + (s_map * s_map);
    std::complex<double> H = one / den;

    return std::arg(H);
}


//nem szukseges fuggvenyek jelenleg
double DelayLineModule::getTPTDelayAllpass(double frequency, double cutoff) {
    if (frequency <= 0.0 || cutoff < 0.0) return 0.0;

    double omegaFreq = 2.0 * juce::MathConstants<double>::pi * frequency / sampleRate;
    double omegaCut = 2.0 * juce::MathConstants<double>::pi * cutoff / sampleRate;

    double g = std::tan(omegaCut * 0.5);

    double gsq = g * g;
    double cos_w = std::cos(omegaFreq);
    double den = 1.0 + gsq + 2.0 * g * cos_w;

    if (den == 0.0) return 0.0;

    double delay = (2.0 * g * (g + cos_w)) / den;

    return sampleRate / (juce::MathConstants<double>::pi * cutoff);

}

double DelayLineModule::getTPTDelayLow(double frequency, double cutoff) {
    if (frequency <= 0.0 || cutoff < 0.0) return 0.0;

    return sampleRate / (2 * juce::MathConstants<double>::pi * cutoff);
}