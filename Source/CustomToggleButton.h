/*
  ==============================================================================

    CustomToggleButton.h
    Created: 31 Jan 2026 9:27:06pm
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class CustomToggleButton : public juce::ToggleButton
{
public:
    CustomToggleButton() {
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
    }
    ~CustomToggleButton() = default;

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsMouseOver, bool shouldDrawButtonAsDown) override;

private:

};