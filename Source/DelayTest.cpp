/*
  ==============================================================================

    DelayTest.cpp
    Created: 2 Feb 2026 5:27:23pm
    Author:  kadar

  ==============================================================================
*/

#include "DelayTest.h"

void DelayTest::runTest() {
    const float sampleRate = 44100.0f;
    DelayModule delay;

    beginTest("Delay Test Start");
    juce::dsp::ProcessSpec specs{ sampleRate, 512, 2 };
    delay.prepare(specs);

    delay.setParameters(100.0f, 0.0f, 1.0f, 20000.0f);

    juce::AudioBuffer<float> empty(2, 5000);
    empty.clear();
    juce::dsp::AudioBlock<float>emptyBlock(empty);
    juce::dsp::ProcessContextReplacing<float> emptyContext(emptyBlock);
    delay.process(emptyContext); //smoothing miatt

    juce::AudioBuffer<float> buffer(2, 10000);
    buffer.clear();
    buffer.setSample(0, 0, 1.0f);

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    delay.process(context);

    float output = buffer.getSample(0, 0);

    float max = 0.0f;
    int maxI = 0;

    for (int i = 4000; i < 5000; ++i) {
        if (buffer.getSample(0, i) > max) {
            max = buffer.getSample(0, i);
            maxI = i;
            
        }
    }

    expectEquals(output, 0.0f, "Output should be silent (100% w)");
    expect(max > 0.8f, "Delayed signal should appear");
    DBG(maxI);
    expect(std::abs(maxI - 4410) < 5, "Delay timing should be accurate (4410 samples)");


}

DelayTest delayTestInstance;