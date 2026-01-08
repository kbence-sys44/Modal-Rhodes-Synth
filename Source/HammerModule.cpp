/*
  ==============================================================================

    HammerModule.cpp
    Created: 23 Dec 2025 4:16:00pm
    Author:  kadar

  ==============================================================================
*/

#include "HammerModule.h"

void HammerModule::prepareHammer(double sRate) {
    sampleRate = sRate;
}

//utes elinditasa
void HammerModule::triggerHammer(float velocity, float length) {

    //a kalapacs hossza kb 60%-a hurnak
    int dynamicLength = static_cast<int>(length * 0.6f);

    int minLen = static_cast<int>(0.002 * sampleRate);
    int maxLen = static_cast<int>(0.008 * sampleRate);

    remainingSamples = juce::jlimit(minLen, maxLen, dynamicLength);

    currentVelocity = velocity; 

    //kicsi vel -> nagy coeff, nagy vel -> kicsi coeff
    filterCoefficient = 0.95f - (velocity * 0.75f);

    //leutes konzisztencia miatt fazis reset
    random.setSeed(static_cast<juce::int64>(velocity * 1000.0f));
}


//utes lezajlasaert felelos fuggveny
float HammerModule::getNextSample() {
    if (remainingSamples <= 0) return 0.0f;

    remainingSamples--;

    //egyszeru feher zaj generalas
    float noise = (random.nextFloat() * 2.0f - 1.0f);
    
    float filteredNoise = (lastOutput * filterCoefficient) + (noise * (1.0f - filterCoefficient));
    lastOutput = filteredNoise;

    float click = noise * 0.3f * currentVelocity;

    float combined = filteredNoise + click;

    //elhalas - a sample fogyasaval halkul az utes
    float envelope = static_cast<float>(remainingSamples) / (0.008f * static_cast<float>(sampleRate));

    float output = combined * envelope * currentVelocity;

    return output;
}

bool HammerModule::isHammerActive() const {
    return (remainingSamples > 0);
}