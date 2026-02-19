/*
  ==============================================================================

    PreampTest.cpp
    Created: 1 Feb 2026 2:33:22pm
    Author:  kadar

  ==============================================================================
*/

#include "PreampTest.h"

void PreampTest::runTest() {

    const float sampleRate = 44100.0f;
    juce::dsp::ProcessSpec specs{ sampleRate, 512, 2 };

    Preamp preamp;
    preamp.prepare(specs);

    beginTest("Gain Processing");

    juce::AudioBuffer<float> buffer(2, 512);
    
    preamp.setDrive(6.0f);

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    double currentPhase = 0.0;
    const double phaseInc = 2.0 * juce::MathConstants<double>::pi * 440.0 / sampleRate;

    for (int k = 0; k < 20; k++) {

        for (int ch = 0; ch < 2; ++ch) {
            auto* chData = buffer.getWritePointer(ch);
            double phase = currentPhase;

            for (int i = 0; i < 512; ++i) {
                chData[i] = 0.5 * std::sin((float)phase);
                phase += phaseInc;
            }
            
        }

        currentPhase += phaseInc * 512;

        preamp.process(context);
    }
    
    float maxAmp = buffer.getMagnitude(0, 0, 512);

    expect(maxAmp > 0.6f, "Output needs to be amplified (Actual: " + juce::String(maxAmp) + ")");
    expect(!std::isnan(maxAmp), "Preamp output is NaN");


}

PreampTest preampTestInstance;