/*
  ==============================================================================

    PickupTest.cpp
    Created: 1 Feb 2026 12:45:18pm
    Author:  kadar

  ==============================================================================
*/

#include "PickupTest.h"

void PickupTest::runTest() {
    const float sampleRate = 44100.0f;
    juce::dsp::ProcessSpec specs{ sampleRate, 512, 1 };

    PickupModule pickup;

    beginTest("Saturation/Waveshaping Test");
    pickup.prepare(specs);

    pickup.setParameters(0.0f, 0.0f, 20000.0f);
    pickup.setFrequency(50.0f);

    //warmup
    for (int i = 0; i < 2000; ++i) {
        pickup.processSample(0.0f);
    }

    float maxOut = 0.0f;
    float inputAmp = 0.1f;
    double phase = 0.0;
    double inc = (440.0 / 44100.0) * juce::MathConstants<double>::twoPi;
    
    for (int i = 0; i < 100; ++i) {
        float inputSample = inputAmp * (float)std::sin(phase);
        phase += inc;

        float output = std::abs(pickup.processSample(inputSample));
        if (output > maxOut) maxOut = output;
    }

    expectWithinAbsoluteError(maxOut, 0.1f, 0.01f, "Small sine wave should be linear");

    pickup.setParameters(20.0f, 0.0f, 20000.0f);

    for (int i = 0; i < 2000; ++i) {
        pickup.processSample(0.0f);
    }

    float satMaxOut = 0.0f;
    inputAmp = 0.5f;

    for (int i = 0; i < 100; ++i) {
        float inputSample = inputAmp * (float)std::sin(phase);
        phase += inc;

        float output = std::abs(pickup.processSample(inputSample));
        if (output > satMaxOut) satMaxOut = output;
    }

    expect(std::abs(satMaxOut) < 1.0f, "Output should be limited below 1.0f");
    expect(std::abs(satMaxOut) > 0.5f, "Output should be amplified");

}

PickupTest pickupTestInstance;