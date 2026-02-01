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
                       ),
    apvts(*this, nullptr, "Parameters", createParameterLayout())
#endif
{
    //tesztek futtatasahoz
    auto forcelink = &tineTestInstance;
    auto forcelink2 = &hammerTestInstance;
    auto forcelink3 = &pickupTestInstance;
    auto forcelink4 = &tremoloTestInstance;
    auto forcelink5 = &preampTestInstance;

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

juce::AudioProcessorValueTreeState::ParameterLayout ModalRhodesAudioProcessor::createParameterLayout() {

    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    //ADSR
    params.push_back(std::make_unique<juce::AudioParameterFloat>("HAMMER_HARDNESS", "Hammer Hardness", 0.5f, 3.0f, 2.0f)); //Attack;
    params.push_back(std::make_unique<juce::AudioParameterFloat>("SUSTAIN_DECAY", "Sustain, Decay", 0.1f, 3.0f, 1.0f)); //lecsenges
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DAMPER_RELEASE", "Release Time", 0.001f, 0.01f, 0.005f));

    //pickup
    params.push_back(std::make_unique<juce::AudioParameterFloat>("PICKUP_SYMMETRY", "Symmetry", 0.0f, 8.0f, 6.0f));

    //preamp
    params.push_back(std::make_unique<juce::AudioParameterFloat>("PREAMP_DRIVE", "Drive", 0.5f, 3.0f, 2.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("PREAMP_BASS", "Bass", 0.5f, 6.0f, 3.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("PREAMP_TREBLE", "Treble", 0.5f, 3.0f, 1.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("OUTPUT_GAIN", "Output Gain", 0.5f, 2.0f, 1.0f));

    //tremolo
    params.push_back(std::make_unique<juce::AudioParameterFloat>("TREM_DEPTH", "Tremolo Depth", 0.0f, 2.0f, 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("TREM_RATE", "Tremolo Rate", 0.5f, 3.0f, 1.4f));

    //reverb
    params.push_back(std::make_unique<juce::AudioParameterFloat>("WET_LEVEL", "Wet Level", 0.0f, 2.0f, 0.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("DRY_LEVEL", "Dry Level", 0.0f, 2.0f, 0.8f));

    //on off
    params.push_back(std::make_unique<juce::AudioParameterBool>("TREM_SWITH", "Tremolo Switch", true));
    params.push_back(std::make_unique<juce::AudioParameterBool>("REVERB_SWITCH", "Reverb Switch", true));
    params.push_back(std::make_unique<juce::AudioParameterBool>("CABINET_SWITCH", "Cabinet Switch", true));
    params.push_back(std::make_unique<juce::AudioParameterBool>("DELAY_SWITCH", "Delay Switch", true));

    return { params.begin(), params.end() };

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
    float hardness = *apvts.getRawParameterValue("HAMMER_HARDNESS");
    float decayMult = *apvts.getRawParameterValue("SUSTAIN_DECAY");
    float release = *apvts.getRawParameterValue("DAMPER_RELEASE");
    float symmetry = *apvts.getRawParameterValue("PICKUP_SYMMETRY");

    float drive = *apvts.getRawParameterValue("PREAMP_DRIVE");
    float bass = *apvts.getRawParameterValue("PREAMP_BASS");
    float treble = *apvts.getRawParameterValue("PREAMP_TREBLE");
    float outputGain = *apvts.getRawParameterValue("OUTPUT_GAIN");

    float depth = *apvts.getRawParameterValue("TREM_DEPTH");
    float rate = *apvts.getRawParameterValue("TREM_RATE");

    float wet = *apvts.getRawParameterValue("WET_LEVEL");
    float dry = *apvts.getRawParameterValue("DRY_LEVEL");

    bool isTremoloOn = *apvts.getRawParameterValue("TREM_SWITH") > 0.5f;
    bool isReverbOn = *apvts.getRawParameterValue("REVERB_SWITCH") > 0.5f;
    bool isCabinetOn = *apvts.getRawParameterValue("CABINET_SWITCH") > 0.5f;
    bool isDelayOn = *apvts.getRawParameterValue("DELAY_SWITCH") > 0.5f;
    
    for (int i = 0; i < rhodesSynth.getNumVoices(); ++i) {
        if (auto* voice = dynamic_cast<RhodesVoice*>(rhodesSynth.getVoice(i))) {
            voice->setHardness(hardness);
            voice->setDecay(decayMult);
            voice->setRelease(release);
            voice->getPickup().setParameters(9.0f, symmetry, 6000.0f);

            preamp.setDrive(drive);
            preamp.setBassGain(bass);
            preamp.setTrebleGain(treble);
            preamp.setOutputLevel(outputGain);

            voice->getTremolo().setDepth(depth);
            voice->getTremolo().setTremRate(rate);
            voice->getTremolo().setTremoloState(isTremoloOn);

            reverbParams.wetLevel = wet;
            reverbParams.dryLevel = dry;
            reverb.setParameters(reverbParams);
        }
    }

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
    if(isReverbOn) cabinet.process(context);
    if(isCabinetOn) reverb.process(context);

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
