/*
  ==============================================================================

    TineTest.cpp
    Created: 1 Feb 2026 12:34:44pm
    Author:  kadar

  ==============================================================================
*/

#include "TineTest.h"


void TineTest::runTest() {
    const float sampleRate = 44100.0f;
    
    ModalTine tine;
    juce::dsp::ProcessSpec specs{ sampleRate, 512, 1 };

    beginTest("Initialization Test");
    tine.setParams(440.0f, 1.0f, 1.0f);

    expectEquals(tine.getCurrentPos(), 0.0f);
    expectEquals(tine.process(0.0f), 0.0f);

    beginTest("Impulse Response Test");

    float output = tine.process(100.0f);
    expect(std::abs(output) > 0.0f, "Tine should only move after force is applied");
    expect(!std::isnan(output), "Output should not be NaN");

    beginTest("Energy Decay Test");

    float maxAmpStart = 0.0f;
    float maxAmpEnd = 0.0f;

    for (int i = 0; i < 1000; ++i)
    {
        float val = std::abs(tine.process(0.0f));
        if (val > maxAmpStart) maxAmpStart = val;
    }

    for (int i = 0; i < 40000; ++i) { //idougras
        tine.process(0.0f);
    }

    for (int i = 0; i < 1000; ++i) { //utolso 1000 minta
        float val = std::abs(tine.process(0.0f));
        if (val > maxAmpEnd) maxAmpEnd = val;
    }

    expect(maxAmpStart > 0.0f, "Start amp should be positive");
    expect(maxAmpEnd < maxAmpStart, "Signal should decay over time");
    expect(!std::isnan(maxAmpEnd), "Signal should not explode (NaN)");

}

TineTest tineTestInstance;