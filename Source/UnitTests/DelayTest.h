/*
  ==============================================================================

    DelayTest.h
    Created: 2 Feb 2026 5:27:23pm
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../Effects/DelayModule.h"

class DelayTest : public juce::UnitTest {
public:
    DelayTest() : juce::UnitTest("Delay Test") {}
    void runTest() override;
private:
    JUCE_LEAK_DETECTOR(DelayTest)
};

extern DelayTest delayTestInstance;