/*
  ==============================================================================

    TestRunner.h
    Created: 21 Dec 2025 6:00:51pm
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "DelayLineTest.h"


class CustomTestRunner : public juce::UnitTestRunner
{
public:
    CustomTestRunner() = default;

    void runAllTests() {
        results.clear();

        logMessage("- Begin Unit Tests -");

        this->juce::UnitTestRunner::runAllTests();
        logMessage("- Tests Finished -");
    }

    juce::String getFullResults() const { return results; }

    void logMessage(const juce::String& message) override {
        results += message + "\n";
        DBG(message);
    }

private:
    juce::String results;
};
