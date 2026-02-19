/*
  ==============================================================================

    ConvolutionCabinet.cpp
    Created: 17 Jan 2026 8:32:00pm
    Author:  kadar

  ==============================================================================
*/

//ez a modul azt szimulalja azt mintha a hang egy gitarerositorol szolna
//nem szukseges, de segiti a valosagos hangzast
//betoltott IR minta fajlbol vetiti a szintetizalt hangra az erosito jelleget

#include "ConvolutionCabinet.h"

void ConvolutionCabinet::prepare(const juce::dsp::ProcessSpec& specs) {
    sampleRate = specs.sampleRate;
    cabinetConv.prepare(specs);

    cabinetConv.loadImpulseResponse( //IR minta alapjan mukodik
        BinaryData::fender_deluxe_big_wav,
        BinaryData::fender_deluxe_big_wavSize,
        juce::dsp::Convolution::Stereo::yes,
        juce::dsp::Convolution::Trim::yes,
        0,
        juce::dsp::Convolution::Normalise::yes
    );

    reset();
}

void ConvolutionCabinet::reset() {
    cabinetConv.reset();
}

void ConvolutionCabinet::process(juce::dsp::ProcessContextReplacing<float>& context) {
    cabinetConv.process(context);

   /* auto& block = context.getOutputBlock();
    for (int ch = 0; ch < block.getNumChannels(); ++ch) {
        auto* samples = block.getChannelPointer(ch);
        for (int i = 0; i < block.getNumSamples(); ++i) {
            samples[i] = std::tanh(samples[i] * 1.5f);
        }
    }*/
    context.getOutputBlock().multiplyBy(makeupGain);
}