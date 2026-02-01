/*
  ==============================================================================

    TremoloTest.h
    Created: 1 Feb 2026 2:22:20pm
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Tremolo.h"

class TremoloTest : public juce::UnitTest {
public:
    TremoloTest() : juce::UnitTest("Tremolo Test") {}
    void runTest() override;
private:
    JUCE_LEAK_DETECTOR(TremoloTest)
};

extern TremoloTest tremoloTestInstance;