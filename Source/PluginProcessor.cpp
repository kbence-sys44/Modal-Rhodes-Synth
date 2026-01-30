/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ModalRhodesAudioProcessor::ModalRhodesAudioProcessor()
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
    //tesztek futtatasahoz
   // auto forcelink = &delayLineTestInstance;
   // auto forcelink2 = &hammerTestInstance;

    //16 hang hozzaadasa (polifonia)
    for (int i = 0; i < 16; ++i) {
        rhodesSynth.addVoice(new RhodesVoice());
    }

    rhodesSynth.addSound(new RhodesSound());
}

ModalRhodesAudioProcessor::~ModalRhodesAudioProcessor()
{
}

//==============================================================================
const juce::String ModalRhodesAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ModalRhodesAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ModalRhodesAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ModalRhodesAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ModalRhodesAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ModalRhodesAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ModalRhodesAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ModalRhodesAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ModalRhodesAudioProcessor::getProgramName (int index)
{
    return {};
}

void ModalRhodesAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ModalRhodesAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    //specifikaciok
    rhodesSynth.setCurrentPlaybackSampleRate(sampleRate);

    juce::dsp::ProcessSpec specifications;
    specifications.sampleRate = sampleRate;
    specifications.maximumBlockSize = samplesPerBlock;
    specifications.numChannels = getTotalNumOutputChannels();

    for (int i = 0; i < rhodesSynth.getNumVoices(); ++i) {
        if (auto* voice = dynamic_cast<RhodesVoice*>(rhodesSynth.getVoice(i))) {
            voice->prepare(specifications);
        }
    }

    //meg tobb parameter
    preamp.prepare(specifications);

    preamp.setDrive(2.0f);
    preamp.setBassGain(3.0f);
    preamp.setTrebleGain(1.5f);
    preamp.setOutputLevel(1.6f);

    cabinet.prepare(specifications);

    reverb.prepare(specifications);

    reverbParams.roomSize = 0.4f;
    reverbParams.damping = 0.8f;
    reverbParams.wetLevel = 0.2f;
    reverbParams.dryLevel = 0.8f;
    reverbParams.width = 0.4f;
    reverbParams.freezeMode = 0.0f;

    reverb.setParameters(reverbParams);
}

void ModalRhodesAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ModalRhodesAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void ModalRhodesAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    //vizualis billenytu funkcionalitasahoz
    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    buffer.clear();

    //rhodesvoice itt fut le
    rhodesSynth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    //a maradek modul itt dolgozza fel a jelet nem hangonkent, hanem mar a teljes mixen egyszerre
    preamp.process(context);
    cabinet.process(context);
    //reverb.process(context);

}

//==============================================================================
bool ModalRhodesAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ModalRhodesAudioProcessor::createEditor()
{
    return new ModalRhodesAudioProcessorEditor(*this);
}

//==============================================================================
void ModalRhodesAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ModalRhodesAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ModalRhodesAudioProcessor();
}
