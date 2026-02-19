/*
  ==============================================================================

    HammerTest.cpp
    Created: 23 Dec 2025 4:57:43pm
    Author:  kadar

  ==============================================================================
*/

#include "HammerTest.h"


void HammerTest::runTest() {
    const float sampleRate = 44100.0f;
    CrossleyHammer hammer;

    beginTest("Triggering Hammer");
    juce::dsp::ProcessSpec specs{ sampleRate, 512, 1 };
    hammer.prepareHammer(specs);

    expect(!hammer.isHammerActive(), "Hammer should be inactive initially");

    hammer.triggerHammer(1.0f, 0.0f, 60);
    expect(hammer.isHammerActive(), "Hammer should be active after triggering");

    beginTest("Collision Test");

    bool impact = false;
    float force = 0.0f;
    float tinePos = 0.0f;

    for (int i = 0; i < 500; ++i) { //500 minta
        force = hammer.getNextSample(tinePos);
        if (force > 0.0f) {
            impact = true;
            break;
        }
    }

    expect(impact, "Hammer should eventually hit the tine");
    expect(force > 0.0f, "Impact should generate a positive force");
    expect(!std::isnan(force), "Force can't be NaN");

}

HammerTest hammerTestInstance;
