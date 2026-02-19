/*
  ==============================================================================

    RunTestButton.h
    Created: 8 Feb 2026 11:36:28am
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class RunTestButton : public juce::ToggleButton
{
public:
    RunTestButton() {
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
    }
    ~RunTestButton() = default;

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsMouseOver, bool shouldDrawButtonAsDown) override;

private:

    juce::Colour buttonColour{ 227, 242, 253 };

};