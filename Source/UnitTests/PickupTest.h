/*
  ==============================================================================

    PickupTest.h
    Created: 1 Feb 2026 12:45:18pm
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../Sound/PickupModule.h"

class PickupTest : public juce::UnitTest {
public:
    PickupTest() : juce::UnitTest("Pickup Test") {}
    void runTest() override;
private:
    JUCE_LEAK_DETECTOR(PickupTest)
};

extern PickupTest pickupTestInstance;