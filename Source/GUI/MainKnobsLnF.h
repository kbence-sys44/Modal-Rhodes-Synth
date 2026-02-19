/*
  ==============================================================================

    mainKnobs.h
    Created: 4 Feb 2026 8:34:46pm
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class MainKnobsLnF : public juce::LookAndFeel_V4
{
public:
    MainKnobsLnF() = default;
    ~MainKnobsLnF() = default;

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosition,
        const float rotationStartAngle, const float rotationEndAngle, juce::Slider& slider) override;


private:

    

};