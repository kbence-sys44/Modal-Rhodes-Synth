/*
  ==============================================================================

    DWMSound.h
    Created: 25 Dec 2025 2:57:53pm
    Author:  kadar

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class DWMSound : public juce::SynthesiserSound
{
public:
    DWMSound() {}
    //minden midi hangra 	csatornra 	rv	nyes
    bool appliesToNote(int /*midiNoteNumber*/) override { return true; }
    bool appliesToChannel(int /*midiChannel*/) override { return true; }
};