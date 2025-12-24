/*
  ==============================================================================

    HammerTest.h
    Created: 23 Dec 2025 4:57:43pm
    Author:  kadar

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "HammerModule.h"

class HammerTest : public juce::UnitTest{
public:
    HammerTest() : juce::UnitTest("Hammer Test") {}
    void runTest() override;
private:
    JUCE_LEAK_DETECTOR(HammerTest)
};

extern HammerTest hammerTestInstance;