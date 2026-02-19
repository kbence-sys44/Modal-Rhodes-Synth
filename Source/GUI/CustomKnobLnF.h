/*
  ==============================================================================

    CustomKnob.h
    Created: 2 Feb 2026 7:15:46pm
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class CustomKnob : public juce::LookAndFeel_V4
{
public:
    CustomKnob() = default;
    ~CustomKnob() = default;

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosition,
        const float rotationStartAngle, const float rotationEndAngle, juce::Slider& slider) override;


private:
    juce::Colour highlightColour{ 201, 44, 60 };
};
