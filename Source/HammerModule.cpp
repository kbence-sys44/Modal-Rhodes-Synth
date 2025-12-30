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

//ütés elinítása
void HammerModule::triggerHammer(float velocity) {

    // az ideális leütési idõtartam 5-10ms
    // jelen esetben 8ms
    remainingSamples = static_cast<int>(0.008 * sampleRate); 
    currentVelocity = velocity; 

    //kicsi vel -> nagy coeff, nagy vel -> kicsi coeff
    filterCoefficient = 0.95f - (velocity * 0.75f);

    //leütés konzisztencia miatt fázis reset
    random.setSeed(static_cast<juce::int64>(velocity * 1000.0f));
}


//ütés lezajlásáért felelõs függvény
float HammerModule::getNextSample() {

    if (remainingSamples <= 0) return 0.0f;

    remainingSamples--;

    //egyszerû fehér zaj generálás
    float noise = (random.nextFloat() * 2.0f - 1.0f);

    float filteredNoise = (lastOutput * filterCoefficient) + (noise * (1.0f - filterCoefficient));
    lastOutput = filteredNoise;

    //elhalás - a sample fogyásával halkul az ütés
    float envelope = static_cast<float>(remainingSamples) / (0.008f * static_cast<float>(sampleRate));

    float output = noise * envelope * currentVelocity;

    return output;
}

bool HammerModule::isHammerActive() const {
    return (remainingSamples > 0);
}