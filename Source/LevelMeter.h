/*
  ==============================================================================

    LevelMeter.h
    Created: 4 Feb 2026 7:17:15pm
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class LevelMeter : public juce::Component
{
public:

    void paint(juce::Graphics& g) override;
    void setLevel(float newLevel);

private:

    float level = 0.0f;
};