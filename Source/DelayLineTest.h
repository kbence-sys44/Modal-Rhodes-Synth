/*
  ==============================================================================

    DelayLineTest.h
    Created: 21 Dec 2025 5:23:23pm
    Author:  kadar

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DelayLineModule.h"

class DelayLineTest : public juce::UnitTest
{
    
public:
    DelayLineTest() : juce::UnitTest("Delay Line Test") {}
    void runTest() override;

private:
    JUCE_LEAK_DETECTOR(DelayLineTest)
};


extern DelayLineTest delayLineTestInstance;