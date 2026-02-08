/*
  ==============================================================================

    DebugButton.h
    Created: 8 Feb 2026 11:17:38am
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class DebugButton : public juce::ToggleButton
{
public:
    DebugButton() {
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
    }
    ~DebugButton() = default;

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsMouseOver, bool shouldDrawButtonAsDown) override;

private:

};