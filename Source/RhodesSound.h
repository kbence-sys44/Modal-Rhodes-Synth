/*
  ==============================================================================

    RhodesSound.h
    Created: 25 Dec 2025 2:57:53pm
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

//ennek a fajlnak nincs jelentossege csak muszaj felulirni az synthsound-ot hogy lefusson a sajat voice osztaly
class RhodesSound : public juce::SynthesiserSound
{
public:
    RhodesSound() {}
    
    bool appliesToNote(int /*midiNoteNumber*/) override { return true; }
    bool appliesToChannel(int /*midiChannel*/) override { return true; }
};