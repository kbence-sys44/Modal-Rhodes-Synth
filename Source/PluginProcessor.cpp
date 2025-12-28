/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RhodesDWMAudioProcessor::RhodesDWMAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    auto forcelink = &delayLineTestInstance;
    auto forcelink2 = &hammerTestInstance;

    //16 hang hozzáadása (polifónia)
    for (int i = 0; i < 16; ++i) {
        rhodesSynth.addVoice(new DWMVoice());
    }

    rhodesSynth.addSound(new DWMSound());
}

RhodesDWMAudioProcessor::~RhodesDWMAudioProcessor()
{
}

//==============================================================================
const juce::String RhodesDWMAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool RhodesDWMAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool RhodesDWMAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool RhodesDWMAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double RhodesDWMAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int RhodesDWMAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int RhodesDWMAudioProcessor::getCurrentProgram()
{
    return 0;
}

void RhodesDWMAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String RhodesDWMAudioProcessor::getProgramName (int index)
{
    return {};
}

void RhodesDWMAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void RhodesDWMAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    rhodesSynth.setCurrentPlaybackSampleRate(sampleRate);

    juce::dsp::ProcessSpec specifications;
    specifications.sampleRate = sampleRate;
    specifications.maximumBlockSize = samplesPerBlock;
    specifications.numChannels = getTotalNumOutputChannels();

    for (int i = 0; i < rhodesSynth.getNumVoices(); ++i) {
        if (auto* voice = dynamic_cast<DWMVoice*>(rhodesSynth.getVoice(i))) {
            voice->prepare(specifications);
        }
    }
}

void RhodesDWMAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool RhodesDWMAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void RhodesDWMAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    //vizuális billenytû funkciónalitásához
    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    buffer.clear();

    rhodesSynth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool RhodesDWMAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* RhodesDWMAudioProcessor::createEditor()
{
    return new RhodesDWMAudioProcessorEditor (*this);
}

//==============================================================================
void RhodesDWMAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void RhodesDWMAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RhodesDWMAudioProcessor();
}
