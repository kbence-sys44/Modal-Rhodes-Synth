/*
  ==============================================================================

    HammerTest.cpp
    Created: 23 Dec 2025 4:57:43pm
    Author:  kadar

  ==============================================================================
*/

#include "HammerTest.h"

void HammerTest::runTest() {
    const double sampleRate = 44100.0;
    HammerModule hammer;
    hammer.prepareHammer(sampleRate);

    beginTest("Trigger, Duration Test");
    {
        float velocity = 1.0f;
        hammer.triggerHammer(velocity);

        expect(hammer.isHammerActive(), "A kalapacsnak aktivnak kell lennie a triggereles utan");


        int sampleCount = 0;
        bool nonZeroSample = false;

        //addig fut ameddig tart az ütés, 8ms - kb 353 minta
        while (hammer.isHammerActive())
        {
            float s = hammer.getNextSample();
            //nem lehetnek 0 értékû sample
            if (std::abs(s) > 0.0f) nonZeroSample = true;
            sampleCount++;

            //végtelen ciklus ellen
            if (sampleCount > 1000) break;
        }

        expect(nonZeroSample, "A kalapacsnak nem nulla mintakat kell generalnia.");
        expectWithinAbsoluteError(sampleCount, 353, 5, "Az utes hossza nem felel meg a vart 8ms-nak.");
        expect(!hammer.isHammerActive(), "A kalapacsnak inaktivnak kell lennie az utes utan.");
    }

    beginTest("Velocity Scale Test");
    {
        //alacsony velocity
        hammer.triggerHammer(0.5f);
        float maxAmpLowVelocity = 0.0f;
        while (hammer.isHammerActive())
        {
            maxAmpLowVelocity = std::max(maxAmpLowVelocity, std::abs(hammer.getNextSample()));
        }

        //magas velocity
        hammer.triggerHammer(1.0f);
        float maxAmpHighVelocity = 0.0f;
        while (hammer.isHammerActive())
        {
            maxAmpHighVelocity = std::max(maxAmpHighVelocity, std::abs(hammer.getNextSample()));
        }

        expectGreaterThan(maxAmpHighVelocity, maxAmpLowVelocity, "A magas velocity tesztnek magasabb amplitudot kell eredmenyeznie.");
        expectLessThan(maxAmpHighVelocity, 1.1f, "Az amplitudo nem haladhatja meg jelentosen az 1.0-at.");

    }

}


HammerTest hammerTestInstance;
