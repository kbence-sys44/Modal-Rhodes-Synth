/*
  ==============================================================================

    PreampTest.h
    Created: 1 Feb 2026 2:33:22pm
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../Effects/Preamp.h"

class PreampTest : public juce::UnitTest {
public:
    PreampTest() : juce::UnitTest("Preamp Test") {}
    void runTest() override;
private:
    JUCE_LEAK_DETECTOR(PreampTest)
};

extern PreampTest preampTestInstance;