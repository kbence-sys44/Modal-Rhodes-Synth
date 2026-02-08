/*
  ==============================================================================

    VolumeFader.cpp
    Created: 6 Feb 2026 6:24:27pm
    Author:  kadar

  ==============================================================================
*/

#include "VolumeFader.h"

void VolumeFader::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos,
    const juce::Slider::SliderStyle style, juce::Slider& slider) {

    if (style != juce::Slider::LinearVertical) { //ha vizsszintes
        LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
        return;
    }

    auto bounds = juce::Rectangle<float>((float)x, (float)y, (float)width, (float)height);
    const float centerX = bounds.getCentreX();

    float trackCenterX = 0.0f;
    if(side == Side::Right) trackCenterX = centerX - (ledBarOffset / 2.0f); //eltolas
    if(side == Side::Left) trackCenterX = centerX + (ledBarOffset / 2.0f);

    juce::Rectangle<float> trackBounds(trackCenterX - trackWidth / 2.0f, (float)y, trackWidth, (float)height);

    //sav
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(trackBounds, trackWidth / 2.0f);
    //arnyek
    g.setColour(juce::Colours::grey.withAlpha(0.3f));
    g.drawRoundedRectangle(trackBounds.reduced(1.0f), trackWidth / 2.0f, 1.0f);


    //ledek
    float ledX = 0.0f;
    if (side == Side::Right) ledX = trackBounds.getRight() + ledBarOffset;
    if (side == Side::Left) ledX = trackBounds.getX() - ledBarOffset;

    float halfThumb = thumbHeight / 2.0f;

    float ledAreaTop = float(y) + halfThumb;
    float ledAreaBottom = float(y) + (float)height - halfThumb;
    float ledMaxHeight = ledAreaBottom - ledAreaTop;
    if (ledMaxHeight < 1.0f) ledMaxHeight = 1.0f;
    float spacing = ledMaxHeight / (float)(ledNum - 1);

    juce::ColourGradient activeLedGrad(juce::Colour(219, 84, 97).brighter(0.3f), ledX, ledAreaBottom,
        juce::Colour(219, 84, 97).darker(0.6f), ledX, ledAreaTop, false);

    for (int i = 0; i < ledNum; ++i) {
        float dotCenterY = ledAreaBottom - (i * spacing);

        bool isActive = (dotCenterY >= sliderPos) ; //- (spacing / 2.0f))

        float radius = ledSize / 2.0f;

        if (isActive) {
            float gradPos = (dotCenterY - ledAreaTop) / ledMaxHeight;

            g.setGradientFill(activeLedGrad);
            //g.setColour(juce::Colour(219, 84, 97));

            g.setOpacity(0.4f);
            g.fillEllipse(ledX - radius - 0.4f, dotCenterY - radius - 0.4f, ledSize + 2.0f, ledSize + 2.0f);

            g.setOpacity(1.0f);
            g.fillEllipse(ledX - radius, dotCenterY - radius, ledSize, ledSize);
        }
        else {
            g.setColour(inactiveLed);
            g.fillEllipse(ledX - radius, dotCenterY - radius, ledSize, ledSize);

            g.setColour(juce::Colours::white.withAlpha(0.3f));
            g.drawEllipse(ledX - radius, dotCenterY - radius, ledSize, ledSize, 0.5f);
        }
    }

    //gomb

    juce::Rectangle<float> thumbBounds;
    thumbBounds.setSize(thumbWidth, thumbHeight);
    thumbBounds.setCentre(trackCenterX, sliderPos);

    juce::ColourGradient thumbGrad(juce::Colour(69, 69, 69), thumbBounds.getTopLeft(),
        juce::Colour(17, 16, 14), thumbBounds.getBottomLeft(), false);
    g.setGradientFill(thumbGrad);
    g.fillRoundedRectangle(thumbBounds, 4.0f);

    //keret
    g.setColour(juce::Colours::black);
    g.drawRoundedRectangle(thumbBounds, 2.0f, 1.5f);

    g.setColour(juce::Colours::black);
    float lineY = thumbBounds.getCentreY();
    g.setColour(juce::Colour(101, 101, 101));
    g.drawLine(thumbBounds.getX() + 3.0f, lineY, thumbBounds.getRight() - 3.0f, lineY, 1.0f);
}