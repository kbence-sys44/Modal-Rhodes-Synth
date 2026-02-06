/*
  ==============================================================================

    VolumeFader.h
    Created: 6 Feb 2026 6:24:27pm
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class VolumeFader : public juce::LookAndFeel_V4
{
public:
    VolumeFader() {};

    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos,
        const juce::Slider::SliderStyle style, juce::Slider& slider) override;

    static constexpr float trackWidth = 8.0f;
    static constexpr float thumbWidth = 15.0f;
    static constexpr float thumbHeight = 10.0f;
    static constexpr float ledSize = 4.0f;
    static constexpr int ledNum = 20;
    static constexpr float ledBarOffset = 8.0f;

    juce::Colour inactiveLed = juce::Colour(0xff2a2a2a);
};