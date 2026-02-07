/*
  ==============================================================================

    OnOffButton.h
    Created: 7 Feb 2026 12:50:29pm
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

enum Type {
    On,
    Off
};

class OnOffButton : public juce::ToggleButton
{
public:
    OnOffButton() {
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
    }
    ~OnOffButton() = default;

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsMouseOver, bool shouldDrawButtonAsDown) override;

    void setType(Type type) {
        buttonType = type;
    }

private:

    juce::Colour onColour { 61, 163, 93 };
    juce::Colour offColour { 201, 44, 60 };

    Type buttonType = Type::On;

};