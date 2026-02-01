/*
  ==============================================================================

    TremoloTest.cpp
    Created: 1 Feb 2026 2:22:20pm
    Author:  kadar

  ==============================================================================
*/

#include "TremoloTest.h"

void TremoloTest::runTest() {
    
    beginTest("Tremolo LFO Test");

    Tremolo tremolo;
    tremolo.prepare(44100.0);
    tremolo.setTremRate(10.0f);
    tremolo.setDepth(1.0f);

    float input = 1.0f;
    float minL = 1.0f;
    float maxL = 0.0f;

    for (int i = 0; i < 4410; ++i) { //0.1mp
        Stereo out = tremolo.process(input);

        if (out.left < minL) minL = out.left;
        if (out.left > maxL) maxL = out.left;

    }

    expect(minL < 0.9f, "Tremolo should reduce amplitude");
    expect(maxL > 0.1f, "Tremolo should contain signal");
    expect(std::abs(maxL - minL) > 0.5f, "Signal should oscillate significantly");

}

TremoloTest tremoloTestInstance;