/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace SlamThatFader;

namespace {
    auto ClipMax{juce::Decibels::decibelsToGain(-0.1f)};
}

//==============================================================================
SlamThatFaderAudioProcessor::SlamThatFaderAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
,
#else
:
#endif
m_parameters(*this, nullptr, "parameters", createParameterLayout())
{
    for (int i = 0; i < SlamThatFader_Parameters.size(); i++) {
        auto& param = SlamThatFader_Parameters[i];

        m_rawParameterValues[i] = m_parameters.getRawParameterValue(param.parameterID);
    }
}

SlamThatFaderAudioProcessor::~SlamThatFaderAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout SlamThatFaderAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::AudioParameterFloat>> params;
    
    for (int i = 0; i < SlamThatFader_Parameters.size(); i++) {
        auto& param = SlamThatFader_Parameters[i];
        params.push_back(std::make_unique<juce::AudioParameterFloat>(param.parameterID, param.name,
                                                               param.minValue, param.maxValue,
                                                               param.initialValue));
    }
    
    return {params.begin(), params.end()};
}

//==============================================================================
const juce::String SlamThatFaderAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SlamThatFaderAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SlamThatFaderAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SlamThatFaderAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SlamThatFaderAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SlamThatFaderAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SlamThatFaderAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SlamThatFaderAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SlamThatFaderAudioProcessor::getProgramName (int index)
{
    return {};
}

void SlamThatFaderAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SlamThatFaderAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
}

void SlamThatFaderAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SlamThatFaderAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void SlamThatFaderAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    auto currentGain = m_rawParameterValues[ParamIndex::Gain]->load();
    
    currentGain = juce::Decibels::decibelsToGain(currentGain);
    
    // todo: less shitty smoothing
    if (currentGain == m_prevGain) {
        buffer.applyGain(currentGain);
    } else {
        buffer.applyGainRamp(0, buffer.getNumSamples(), m_prevGain, currentGain);
        m_prevGain = currentGain;
    }
    
    // clip
    for (auto ch = 0; ch < buffer.getNumChannels(); ch++) {
        juce::FloatVectorOperations::clip(buffer.getWritePointer(ch),
                                          buffer.getWritePointer(ch),
                                          -ClipMax, ClipMax, buffer.getNumSamples());
    }
}

//==============================================================================
bool SlamThatFaderAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* SlamThatFaderAudioProcessor::createEditor()
{
    return new SlamThatFaderAudioProcessorEditor(*this, m_parameters);
}

//==============================================================================
void SlamThatFaderAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = m_parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary(*xml, destData);
}

void SlamThatFaderAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
 
    if (xmlState.get() != nullptr) {
        if (xmlState->hasTagName (m_parameters.state.getType())) {
            m_parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
        }
    }
            
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SlamThatFaderAudioProcessor();
}
