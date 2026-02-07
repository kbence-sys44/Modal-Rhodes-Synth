/*
  ==============================================================================

    VolumeFader.h
    Created: 6 Feb 2026 6:24:27pm
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

enum class Side {
    Left,
    Right
};

class VolumeFader : public juce::LookAndFeel_V4
{
public:
    VolumeFader() {};

    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos,
        const juce::Slider::SliderStyle style, juce::Slider& slider) override;

    static constexpr float trackWidth = 10.0f;
    static constexpr float thumbWidth = 25.0f;
    static constexpr float thumbHeight = 15.0f;
    static constexpr float ledSize = 8.0f;
    static constexpr int ledNum = 20;
    static constexpr float ledBarOffset = 20.0f;
    
    Side side = Side::Right;

    juce::Colour inactiveLed = juce::Colour(0xff2a2a2a);
};