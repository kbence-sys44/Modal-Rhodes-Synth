/*
  ==============================================================================

    TineTest.h
    Created: 1 Feb 2026 12:34:44pm
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../Sound/ModalTine.h"

class TineTest : public juce::UnitTest {
public:
    TineTest() : juce::UnitTest("Tine Test") {}
    void runTest() override;
private:
    JUCE_LEAK_DETECTOR(TineTest)
};

extern TineTest tineTestInstance;